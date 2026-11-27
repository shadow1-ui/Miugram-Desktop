/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
#ifdef _WIN32
#include "archive_dpapi.h"
#include "archive_policy.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <limits>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Miugram {
// All calls perform file I/O and must run away from the UI thread. Snapshot capture,
// display, logout handling, and settings are not wired up yet. Protected data is bound
// to the Windows user, not to the app's local passcode.
struct ArchivedText {
    std::int64_t dialog = 0, sender = 0;
    std::uint32_t id = 0, date = 0;
    std::string text;
};
class WindowsTextArchive {
public:
    WindowsTextArchive(std::wstring path, std::uint64_t account)
    : _path(std::move(path)), _account(account) {}

    bool put(const ArchivedText &record, const ArchiveEvidence &evidence) {
        return putBatch({ { record, evidence } });
    }
    // All evidence is checked before modifying storage; one DPAPI transaction
    // covers the entire deletion update. Caller must revalidate live policy.
    bool putBatch(const std::vector<std::pair<ArchivedText, ArchiveEvidence>> &batch) {
        if (!_account) return false;
        for (const auto &[record, evidence] : batch) {
            if (!MayRetainText(evidence) || !Valid(record)) return false;
        }
        if (batch.empty()) return true;
        const auto guard = std::lock_guard(_mutex);
        auto entries = Read();
        if (!entries) return false; // Never replace an unreadable/corrupt archive.
        for (const auto &[record, evidence] : batch) {
            entries->erase(std::remove_if(entries->begin(), entries->end(), [&](const auto &old) {
                return old.dialog == record.dialog && old.id == record.id;
            }), entries->end());
            entries->push_back(record);
        }
        // Keep the most recent records within the count and serialized byte limits.
        // Header is 8 bytes; each record has 28 bytes of fixed-width fields.
        std::size_t first = entries->size(), bytes = 8;
        while (first > 0 && entries->size() - first < kMaxRecords) {
            const auto itemBytes = std::size_t(28) + (*entries)[first - 1].text.size();
            if (itemBytes > kArchiveMaxPlaintext - bytes) break;
            bytes += itemBytes;
            --first;
        }
        entries->erase(entries->begin(), entries->begin() + first);
        return Write(*entries);
    }
    std::optional<std::vector<ArchivedText>> forDialog(std::int64_t dialog, const ArchiveEvidence &evidence) const {
        if (!_account || !dialog || !MayDisplayArchive(evidence)) return std::nullopt;
        const auto guard = std::lock_guard(_mutex);
        auto entries = Read();
        if (!entries) return std::nullopt;
        entries->erase(std::remove_if(entries->begin(), entries->end(), [&](const auto &entry) {
            return entry.dialog != dialog;
        }), entries->end());
        return entries;
    }
    bool removeDialog(std::int64_t dialog) {
        return RemoveMatching(dialog, 0);
    }
    bool removeMessage(std::int64_t dialog, std::uint32_t messageId) {
        return messageId ? RemoveMatching(dialog, messageId) : false;
    }
    bool clear() {
        if (!_account) return false;
        const auto guard = std::lock_guard(_mutex);
        return Delete();
    }
private:
    bool RemoveMatching(std::int64_t dialog, std::uint32_t messageId) {
        if (!_account || !dialog) return false;
        const auto guard = std::lock_guard(_mutex);
        auto entries = Read();
        if (!entries) return false;
        const auto before = entries->size();
        entries->erase(std::remove_if(entries->begin(), entries->end(), [&](const auto &entry) {
            return entry.dialog == dialog && (!messageId || entry.id == messageId);
        }), entries->end());
        if (entries->size() == before) return true;
        return entries->empty() ? Delete() : Write(*entries);
    }
    static constexpr std::size_t kMaxRecords = 2000;
    static constexpr std::size_t kMaxTextBytes = 16 * 1024;
    static bool Valid(const ArchivedText &r) {
        return r.dialog && r.id && r.date && !r.text.empty() && r.text.size() <= kMaxTextBytes;
    }
    static void Number(ArchiveBytes &out, std::uint64_t value, int size) {
        for (int i = 0; i != size; ++i) out.push_back(std::uint8_t(value >> (i * 8)));
    }
    static bool Number(const ArchiveBytes &in, std::size_t &at, int size, std::uint64_t &value) {
        if (at > in.size() || std::size_t(size) > in.size() - at) return false;
        value = 0;
        for (int i = 0; i != size; ++i) value |= std::uint64_t(in[at++]) << (i * 8);
        return true;
    }
    std::optional<std::vector<ArchivedText>> Read() const {
        const auto handle = CreateFileW(_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            const auto error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return std::vector<ArchivedText>();
            return std::nullopt;
        }
        LARGE_INTEGER size{};
        if (!GetFileSizeEx(handle, &size) || size.QuadPart <= 0
                || size.QuadPart > static_cast<LONGLONG>(kArchiveMaxCiphertext)) {
            CloseHandle(handle); return std::nullopt;
        }
        ArchiveBytes encrypted(static_cast<std::size_t>(size.QuadPart));
        DWORD read = 0;
        const auto success = ReadFile(handle, encrypted.data(), DWORD(encrypted.size()), &read, nullptr);
        CloseHandle(handle);
        if (!success || read != encrypted.size()) return std::nullopt;
        auto plain = UnprotectArchive(encrypted, _account);
        if (!plain) return std::nullopt;
        const auto result = Parse(*plain);
        SecureZeroMemory(plain->data(), plain->size());
        return result;
    }
    static std::optional<std::vector<ArchivedText>> Parse(const ArchiveBytes &plain) {
        if (plain.size() < 8 || plain[0] != 'M' || plain[1] != 'T' || plain[2] != 'X' || plain[3] != 1) return std::nullopt;
        std::size_t at = 4;
        std::uint64_t count = 0;
        if (!Number(plain, at, 4, count) || count > kMaxRecords) return std::nullopt;
        std::vector<ArchivedText> result;
        result.reserve(std::size_t(count));
        for (std::size_t i = 0; i != count; ++i) {
            std::uint64_t dialog, sender, id, date, length;
            if (!Number(plain, at, 8, dialog) || !Number(plain, at, 8, sender)
                    || !Number(plain, at, 4, id) || !Number(plain, at, 4, date)
                    || !Number(plain, at, 4, length) || length > kMaxTextBytes
                    || at > plain.size() || length > plain.size() - at) return std::nullopt;
            ArchivedText entry{ std::int64_t(dialog), std::int64_t(sender), std::uint32_t(id), std::uint32_t(date),
                std::string(reinterpret_cast<const char*>(plain.data() + at), std::size_t(length)) };
            at += std::size_t(length);
            if (!Valid(entry)) return std::nullopt;
            result.push_back(std::move(entry));
        }
        if (at != plain.size()) return std::nullopt;
        return result;
    }
    bool Write(const std::vector<ArchivedText> &entries) const {
        ArchiveBytes plain{ 'M', 'T', 'X', 1 };
        Number(plain, entries.size(), 4);
        for (const auto &entry : entries) {
            if (!Valid(entry) || plain.size() + 28 + entry.text.size() > kArchiveMaxPlaintext) {
                SecureZeroMemory(plain.data(), plain.size());
                return false;
            }
            Number(plain, std::uint64_t(entry.dialog), 8); Number(plain, std::uint64_t(entry.sender), 8);
            Number(plain, entry.id, 4); Number(plain, entry.date, 4); Number(plain, entry.text.size(), 4);
            plain.insert(plain.end(), entry.text.begin(), entry.text.end());
        }
        const auto encrypted = ProtectArchive(plain, _account);
        SecureZeroMemory(plain.data(), plain.size());
        if (!encrypted) return false;
        static std::atomic_uint64_t serial{0};
        const auto temporary = _path + L".tmp-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(++serial);
        const auto handle = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE) return false;
        DWORD written = 0;
        const auto ok = WriteFile(handle, encrypted->data(), DWORD(encrypted->size()), &written, nullptr)
            && written == encrypted->size() && FlushFileBuffers(handle);
        CloseHandle(handle);
        if (ok && MoveFileExW(temporary.c_str(), _path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) return true;
        DeleteFileW(temporary.c_str());
        return false;
    }
    bool Delete() const {
        if (DeleteFileW(_path.c_str())) return true;
        const auto error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    }
    std::wstring _path;
    std::uint64_t _account;
    inline static std::mutex _mutex;
};
} // namespace Miugram
#endif // _WIN32

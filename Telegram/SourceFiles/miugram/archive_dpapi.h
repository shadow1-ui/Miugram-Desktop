/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wincrypt.h>
#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace Miugram {
// Windows user-bound protection, not protection against the logged-in Windows user.
// This is a codec only: the archive itself and its UI are not implemented yet.
constexpr std::size_t kArchiveMaxPlaintext = 4 * 1024 * 1024;
constexpr std::size_t kArchiveMaxCiphertext = kArchiveMaxPlaintext + 65536;
using ArchiveBytes = std::vector<std::uint8_t>;
inline std::array<std::uint8_t, 12> ArchiveEntropy(std::uint64_t account) {
    std::array<std::uint8_t, 12> bytes{ 'M', 'I', 'U', 1 };
    for (int i = 0; i != 8; ++i) bytes[4 + i] = std::uint8_t(account >> (i * 8));
    return bytes;
}
inline std::optional<ArchiveBytes> ProtectArchive(const ArchiveBytes &plain, std::uint64_t account) {
    if (!account || plain.empty() || plain.size() > kArchiveMaxPlaintext) return std::nullopt;
    auto entropy = ArchiveEntropy(account);
    DATA_BLOB data{ DWORD(plain.size()), const_cast<BYTE*>(plain.data()) };
    DATA_BLOB extra{ DWORD(entropy.size()), entropy.data() };
    DATA_BLOB output{};
    if (!CryptProtectData(&data, L"Miugram archive", &extra, nullptr, nullptr,
            CRYPTPROTECT_UI_FORBIDDEN, &output)) return std::nullopt;
    std::optional<ArchiveBytes> result;
    if (output.cbData && output.cbData <= kArchiveMaxCiphertext) {
        result = ArchiveBytes(output.pbData, output.pbData + output.cbData);
    }
    LocalFree(output.pbData);
    return result;
}
inline std::optional<ArchiveBytes> UnprotectArchive(const ArchiveBytes &cipher, std::uint64_t account) {
    if (!account || cipher.empty() || cipher.size() > kArchiveMaxCiphertext) return std::nullopt;
    auto entropy = ArchiveEntropy(account);
    DATA_BLOB data{ DWORD(cipher.size()), const_cast<BYTE*>(cipher.data()) };
    DATA_BLOB extra{ DWORD(entropy.size()), entropy.data() };
    DATA_BLOB output{};
    if (!CryptUnprotectData(&data, nullptr, &extra, nullptr, nullptr,
            CRYPTPROTECT_UI_FORBIDDEN, &output)) return std::nullopt;
    std::optional<ArchiveBytes> result;
    if (output.cbData && output.cbData <= kArchiveMaxPlaintext) {
        result = ArchiveBytes(output.pbData, output.pbData + output.cbData);
    }
    if (output.pbData) {
        SecureZeroMemory(output.pbData, output.cbData);
        LocalFree(output.pbData);
    }
    return result;
}
} // namespace Miugram
#endif // _WIN32

#include "Telegram/SourceFiles/miugram/archive_windows.h"
#include <cassert>
#include <iostream>
int main() {
    wchar_t directory[MAX_PATH], temp[MAX_PATH];
    assert(GetTempPathW(MAX_PATH, directory));
    assert(GetTempFileNameW(directory, L"miu", 0, temp));
    assert(DeleteFileW(temp));
    Miugram::WindowsTextArchive archive(temp, 1234), other(temp, 1235);
    auto evidence = Miugram::ArchiveEvidence{};
    const Miugram::ArchivedText first{ -123, 12, 1, 1700000000, "synthetic original" };
    assert(!archive.put(first, evidence));
    evidence.optedIn = evidence.serverDeletion = evidence.incoming = true;
    evidence.ordinaryCloudChat = evidence.peerPolicyKnown = evidence.ordinaryTextOnly = true;
    evidence.peerProtected = evidence.messageProtected = evidence.hasExpiry = false;
    assert(archive.put(first, evidence));
    assert(archive.put({ -123, 12, 1, 1700000000, "synthetic replacement" }, evidence));
    assert(archive.put({ 77, 10, 2, 1700000001, "different chat" }, evidence));
    const auto rows = archive.forDialog(-123, evidence);
    assert(rows && rows->size() == 1 && rows->front().text == "synthetic replacement");
    evidence.peerProtected = true;
    assert(!archive.forDialog(-123, evidence));
    evidence.peerProtected = false;
    assert(!other.forDialog(-123, evidence)); // Different account cannot decrypt this file.
    assert(archive.removeMessage(-123, 1));
    assert(archive.forDialog(-123, evidence)->empty());
    assert(archive.forDialog(77, evidence)->size() == 1);
    assert(archive.put({ -123, 12, 1, 1700000000, "synthetic replacement" }, evidence));
    assert(archive.removeDialog(77));
    assert(archive.forDialog(77, evidence)->empty());
    assert(archive.forDialog(-123, evidence)->size() == 1);
    const auto handle = CreateFileW(temp, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    assert(handle != INVALID_HANDLE_VALUE);
    const auto size = GetFileSize(handle, nullptr);
    Miugram::ArchiveBytes bytes(size);
    DWORD transferred = 0;
    assert(ReadFile(handle, bytes.data(), size, &transferred, nullptr) && transferred == size);
    const std::string leaked(bytes.begin(), bytes.end());
    assert(leaked.find("synthetic replacement") == std::string::npos);
    bytes.back() ^= 1;
    assert(SetFilePointer(handle, 0, nullptr, FILE_BEGIN) == 0);
    assert(WriteFile(handle, bytes.data(), size, &transferred, nullptr) && transferred == size);
    CloseHandle(handle);
    assert(!archive.forDialog(-123, evidence));
    assert(!archive.put(first, evidence)); // A damaged archive must not be silently replaced.
    assert(archive.clear());
    assert(archive.forDialog(-123, evidence)->empty());
    std::cout << "Synthetic protected storage, account isolation and cleanup passed.\n";
}

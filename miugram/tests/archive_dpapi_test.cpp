#include "Telegram/SourceFiles/miugram/archive_dpapi.h"
#include <cassert>
#include <iostream>
int main() {
    const Miugram::ArchiveBytes plain{ 's', 'y', 'n', 't', 'h', 'e', 't', 'i', 'c' };
    const auto encrypted = Miugram::ProtectArchive(plain, 17);
    assert(encrypted && *encrypted != plain);
    const auto decoded = Miugram::UnprotectArchive(*encrypted, 17);
    assert(decoded && *decoded == plain);
    assert(!Miugram::UnprotectArchive(*encrypted, 18));
    assert(!Miugram::ProtectArchive(plain, 0));
    assert(!Miugram::ProtectArchive({}, 17));
    auto damaged = *encrypted;
    damaged.back() ^= 1;
    assert(!Miugram::UnprotectArchive(damaged, 17));
    damaged.resize(4);
    assert(!Miugram::UnprotectArchive(damaged, 17));
    std::cout << "DPAPI codec: synthetic roundtrip, account isolation and corruption rejection passed.\n";
}

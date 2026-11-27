#include "Telegram/SourceFiles/miugram/archive_policy.h"
#include <cassert>
#include <iostream>
int main() {
    assert(!Miugram::MayRetainText({}));
    auto display = Miugram::ArchiveEvidence{};
    assert(!Miugram::MayDisplayArchive(display));
    display.optedIn = display.ordinaryCloudChat = display.peerPolicyKnown = true;
    display.peerProtected = display.hasExpiry = false;
    assert(Miugram::MayDisplayArchive(display));
    display.peerProtected = true;
    assert(!Miugram::MayDisplayArchive(display));
    display.peerProtected = false; display.hasExpiry = true;
    assert(!Miugram::MayDisplayArchive(display));
    int permitted = 0;
    for (int bits = 0; bits < (1 << 9); ++bits) {
        bool b[9];
        for (int i = 0; i != 9; ++i) b[i] = (bits & (1 << i)) != 0;
        const Miugram::ArchiveEvidence e{ b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8] };
        const auto expected = b[0] && b[1] && b[2] && b[3] && b[4]
            && !b[5] && !b[6] && !b[7] && b[8];
        assert(Miugram::MayRetainText(e) == expected);
        permitted += expected;
    }
    assert(permitted == 1);
    std::cout << "Archive safety policy: 512 states checked; integration not implemented.\n";
}

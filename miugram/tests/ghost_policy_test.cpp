#include "Telegram/SourceFiles/miugram/ghost_policy.h"
#include <cassert>
#include <iostream>
int main() {
    for (bool enabled : { false, true }) {
        for (bool expiring : { false, true }) {
            assert(Miugram::SuppressRead(enabled, expiring) == (enabled && !expiring));
        }
    }
    std::cout << "Desktop ghost policy: all four state combinations passed.\n";
}

#include "Telegram/SourceFiles/miugram/auto_frame_policy.h"
#include <array>
#include <cassert>
#include <iostream>

int main() {
    using Miugram::ChooseAutoFrame;
    auto invalid = ChooseAutoFrame(nullptr, 32, 32, 100, 100);
    assert(invalid.left == 0. && invalid.bottom == 1.);
    std::array<std::uint32_t, 32 * 32> pixels;
    pixels.fill(0xff808080);
    auto portrait = ChooseAutoFrame(pixels.data(), 32, 32, 1000, 1800);
    assert(portrait.left == 0. && portrait.right == 1.);
    assert(portrait.top >= 0. && portrait.bottom <= 1. && portrait.top < portrait.bottom);
    assert(std::abs((portrait.top + portrait.bottom) / 2. - .5) < 1e-9);
    auto wide = ChooseAutoFrame(pixels.data(), 32, 32, 3600, 1000);
    assert(wide.left >= 0. && wide.right <= 1. && wide.left < wide.right);
    assert(wide.top == 0. && wide.bottom == 1.);
    for (int y = 3; y < 12; ++y) for (int x = 3; x < 12; ++x) pixels[y * 32 + x] = (x + y) % 2 ? 0xffffffff : 0xff000000;
    auto detail = ChooseAutoFrame(pixels.data(), 32, 32, 1000, 1800);
    assert(detail.top < portrait.top && detail.top >= 0. && detail.bottom <= 1.);
    std::cout << "Automatic frame bounds and visual-detail tests passed.\n";
}

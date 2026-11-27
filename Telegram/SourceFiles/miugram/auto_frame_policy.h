/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Miugram {
struct AutoFrame { double left, top, right, bottom; };

// Normalized crop, using local visual detail only. The result remains editable.
inline AutoFrame ChooseAutoFrame(const std::uint32_t *pixels, int width, int height, int imageWidth, int imageHeight) {
    if (!pixels || width < 2 || height < 2 || imageWidth < 1 || imageHeight < 1) return { 0., 0., 1., 1. };
    const auto gray = [](std::uint32_t pixel) {
        return int(((pixel >> 16) & 255) * 299 + ((pixel >> 8) & 255) * 587 + (pixel & 255) * 114) / 1000;
    };
    double total = 0., xsum = 0., ysum = 0.;
    std::uint64_t edgeTotal = 0;
    for (int y = 1; y != height; ++y) {
        for (int x = 1; x != width; ++x) {
            const auto i = y * width + x;
            const auto value = gray(pixels[i]);
            const auto edge = std::abs(value - gray(pixels[i - 1])) + std::abs(value - gray(pixels[i - width]));
            edgeTotal += edge;
            const auto nx = (x + .5) / width;
            const auto ny = (y + .5) / height;
            const auto weight = (edge + 2) * (1. - .18 * (std::abs(nx - .5) + std::abs(ny - .5)));
            total += weight; xsum += nx * weight; ysum += ny * weight;
        }
    }
    const auto centerX = (edgeTotal && total > 0.) ? xsum / total : .5;
    const auto centerY = (edgeTotal && total > 0.) ? ysum / total : .5;
    const auto ratio = double(imageWidth) / imageHeight;
    const auto cropW = ratio > 1.8 ? 1.8 / ratio : 1.;
    const auto cropH = ratio > 1.8 ? 1. : ratio / 1.8;
    const auto left = std::clamp(centerX - cropW / 2., 0., 1. - cropW);
    const auto top = std::clamp(centerY - cropH / 2., 0., 1. - cropH);
    return { left, top, left + cropW, top + cropH };
}
} // namespace Miugram

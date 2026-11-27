/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
#include "miugram/auto_frame_policy.h"
#include <QtCore/QRect>
#include <QtGui/QImage>
#include <array>

namespace Miugram {
inline QRect SuggestProfileCrop(const QImage &image) {
    if (image.isNull()) return {};
    const auto sample = image.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_RGB32);
    if (sample.isNull()) return image.rect();
    std::array<std::uint32_t, 32 * 32> pixels{};
    for (int y = 0; y != 32; ++y) for (int x = 0; x != 32; ++x) pixels[y * 32 + x] = sample.pixel(x, y);
    const auto frame = ChooseAutoFrame(pixels.data(), 32, 32, image.width(), image.height());
    const auto left = std::clamp(int(frame.left * image.width()), 0, image.width() - 1);
    const auto top = std::clamp(int(frame.top * image.height()), 0, image.height() - 1);
    const auto right = std::clamp(int(std::round(frame.right * image.width())), left + 1, image.width());
    const auto bottom = std::clamp(int(std::round(frame.bottom * image.height())), top + 1, image.height());
    return QRect(left, top, right - left, bottom - top);
}
} // namespace Miugram

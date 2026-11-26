/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
namespace Miugram {
constexpr bool SuppressRead(bool enabled, bool expiring) {
    return enabled && !expiring;
}
} // namespace Miugram

/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <map>
#include <mutex>

namespace Miugram {
namespace detail {
inline std::mutex &GhostMutex() { static std::mutex mutex; return mutex; }
inline std::map<quint64, bool> &GhostCache() { static std::map<quint64, bool> cache; return cache; }
inline QString GhostKey(quint64 account) { return QString::number(account); }
} // namespace detail

// Local per-account setting; never shared through Telegram cloud settings.
inline bool GhostEnabled(quint64 account) {
    const auto lock = std::lock_guard(detail::GhostMutex());
    auto &cache = detail::GhostCache();
    const auto i = cache.find(account);
    if (i != cache.end()) return i->second;
    QSettings settings(QStringLiteral("Miugram"), QStringLiteral("Preferences"));
    const auto enabled = settings.value(QStringLiteral("ghost/") + detail::GhostKey(account), false).toBool();
    cache.emplace(account, enabled);
    return enabled;
}
inline bool SetGhostEnabled(quint64 account, bool enabled) {
    const auto lock = std::lock_guard(detail::GhostMutex());
    QSettings settings(QStringLiteral("Miugram"), QStringLiteral("Preferences"));
    settings.setValue(QStringLiteral("ghost/") + detail::GhostKey(account), enabled);
    settings.sync();
    if (settings.status() != QSettings::NoError) return false;
    detail::GhostCache()[account] = enabled;
    return true;
}
} // namespace Miugram

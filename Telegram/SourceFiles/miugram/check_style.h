/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
#include <QtCore/QSettings>

namespace Miugram {
inline int CheckStyle() {
    QSettings settings(QStringLiteral("Miugram"), QStringLiteral("Preferences"));
    return settings.value(QStringLiteral("appearance/check_style"), 0).toInt() == 1 ? 1 : 0;
}
inline bool SetCheckStyle(int style) {
    QSettings settings(QStringLiteral("Miugram"), QStringLiteral("Preferences"));
    settings.setValue(QStringLiteral("appearance/check_style"), style == 1 ? 1 : 0);
    settings.sync();
    return settings.status() == QSettings::NoError;
}
} // namespace Miugram

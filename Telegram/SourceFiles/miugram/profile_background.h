/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSaveFile>
#include <QtGui/QImage>

namespace Miugram {
// Stored only in Miugram's local data folder, never sent to Telegram.
inline QString ProfileBackgroundPath(quint64 account) {
    return cWorkingDir() + u"tdata/miugram/profile-%1.png"_q.arg(account);
}
inline QImage LoadProfileBackground(quint64 account) {
    return account ? QImage(ProfileBackgroundPath(account), "PNG") : QImage();
}
inline bool SaveProfileBackground(quint64 account, QImage image) {
    if (!account || image.isNull()) return false;
    if (!QDir().mkpath(cWorkingDir() + u"tdata/miugram"_q)) return false;
    if (image.width() > 1600 || image.height() > 1600) {
        image = image.scaled(1600, 1600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    QSaveFile output(ProfileBackgroundPath(account));
    if (!output.open(QIODevice::WriteOnly) || !image.save(&output, "PNG")) return false;
    return output.commit();
}
inline bool RemoveProfileBackground(quint64 account) {
    if (!account) return false;
    const auto path = ProfileBackgroundPath(account);
    return !QFile::exists(path) || QFile::remove(path);
}
} // namespace Miugram

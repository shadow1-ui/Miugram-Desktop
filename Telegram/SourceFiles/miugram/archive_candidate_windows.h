/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
#ifdef Q_OS_WIN
#include "miugram/archive_windows.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include <QtCore/QByteArray>
#include <optional>

namespace Miugram {
// Extract a conservative candidate in the main thread, before a server deletion
// destroys the item. This helper is NOT yet connected to the deletion or chat UI.
// Server deletion updates do not identify which device or person initiated them.
inline std::optional<ArchivedText> CandidateFromServerDeletion(
        not_null<HistoryItem*> item, bool optedIn, bool serverDeletion) {
    const auto peer = item->history()->peer;
    const auto &body = item->originalText();
    auto evidence = ArchiveEvidence{};
    evidence.optedIn = optedIn;
    evidence.serverDeletion = serverDeletion;
    evidence.incoming = !item->out();
    evidence.ordinaryCloudChat = (peer->isUser() || peer->isChat() || peer->isChannel()) && !peer->isSelf();
    evidence.peerPolicyKnown = peer->isFullLoaded();
    evidence.peerProtected = !peer->allowsForwarding();
    evidence.messageProtected = item->forbidsForward();
    evidence.hasExpiry = peer->messagesTTL() != 0 || item->ttlDestroyAt() != 0;
    // Rich text is excluded until entities (including spoilers) can be retained safely.
    evidence.ordinaryTextOnly = item->isHistoryEntry() && !item->isLocal() && !item->isService()
        && !item->isScheduled() && !item->isSponsored() && !item->media()
        && !item->Has<HistoryMessageForwarded>() && !body.text.isEmpty() && body.entities.empty();
    if (!MayRetainText(evidence) || item->id.bare <= 0 || item->date() <= 0) return std::nullopt;
    const auto utf8 = body.text.toUtf8();
    if (utf8.isEmpty() || utf8.size() > 16 * 1024) return std::nullopt;
    return ArchivedText{
        std::int64_t(peer->id.value), std::int64_t(item->from()->id.value),
        std::uint32_t(item->id.bare), std::uint32_t(item->date()),
        std::string(utf8.constData(), std::size_t(utf8.size()))
    };
}
} // namespace Miugram
#endif // Q_OS_WIN

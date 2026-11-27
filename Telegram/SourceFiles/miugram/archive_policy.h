/* Miugram additions are licensed under GPL-3.0-or-later with the upstream exception. */
#pragma once
namespace Miugram {
// Defaults fail closed. This policy does not capture, store, or display messages.
struct ArchiveEvidence {
    bool optedIn = false;
    bool serverDeletion = false;
    bool incoming = false;
    bool ordinaryCloudChat = false;
    bool peerPolicyKnown = false;
    bool peerProtected = true;
    bool messageProtected = true;
    bool hasExpiry = true;
    bool ordinaryTextOnly = false;
};
// Check the current chat policy again before exposing existing snapshots.
constexpr bool MayDisplayArchive(const ArchiveEvidence &e) {
    return e.optedIn && e.ordinaryCloudChat && e.peerPolicyKnown
        && !e.peerProtected && !e.hasExpiry;
}
constexpr bool MayRetainText(const ArchiveEvidence &e) {
    return e.optedIn && e.serverDeletion && e.incoming && e.ordinaryCloudChat
        && e.peerPolicyKnown && !e.peerProtected && !e.messageProtected
        && !e.hasExpiry && e.ordinaryTextOnly;
}
} // namespace Miugram

# Miugram for Windows

Miugram is an unofficial Telegram client based on the official upstream source. The upstream README, license and third-party notices remain applicable.

## Baseline

- Upstream: https://github.com/telegramdesktop/tdesktop
- Initial commit: `1fdd0ea143accc8b5d917e95144cfc5f73e06395`
- Project assets in `miugram/assets/logos`: the four supplied concepts and original composite. Four multi-resolution Windows ICO resources have been generated from the supplied concepts; cat and paper plane is the default executable icon. The other three are not yet selectable.

## Current status

A first Miugram entry in Desktop settings displays the current account ID and current DC when available. The username context menu retains the upstream public-link action and adds an ID deep link with an access limitation; the ID link cannot bypass privacy and may not resolve for recipients who do not know the user. It is unbuilt and untested on Windows. No tested Miugram builds or downloadable releases are available. The Windows executable, app-data name, shortcut name, notification identifiers and default executable icon have been changed for Miugram, and the upstream auto-updater is disabled by default. These changes are untested on Windows and do not establish complete data isolation. The installer, signing identity, API credentials, and remaining branding need work before distribution. Do not use the upstream installer to distribute Miugram. Never commit credentials or signing keys. Follow the upstream build instructions and supply your own Telegram API ID and hash.

## Planned features

A cohesive dark graphite and muted pink design; fast, consistent animations; a dedicated Miugram settings area; account ID and real current DC; user links with privacy limitations; configurable message status indicators; local profile background with cropping; and selectable app icons where supported. Custom interface icon design is deferred pending approval.

The requested option to retain locally received messages deleted by others must be clearly marked, safely stored and deletable. It cannot recover messages never received and must not bypass self-destructing or protected content.

## Experimental ghost mode (Desktop, not yet tested)

A local per-account opt-in attempts to suppress ordinary history, discussion and saved-history reads, ordinary content reads, and explicit mark-all mentions/reactions. Queued history, discussion and saved-history requests are checked again at send time. Expiring media content reads are not suppressed. Stories, typing/online status, outgoing messages, other sessions and already-sent requests are not covered. Local unread badges can remain visible. This is not a guarantee of invisibility. Real two-account network tests, including timers, toggles and queue races, are required before release; the code is not yet compiled or tested on Windows.

Telegram's API terms explicitly prohibit manipulating read statuses, including ghost mode; violation can result in loss of API access. See https://core.telegram.org/api/terms.

## Build verification

The `Miugram Windows debug build` workflow attempts the upstream Windows dependency preparation and a debug compile with `TDESKTOP_API_TEST`. It publishes no executable and is not a release pipeline. A successful compile would not verify login, calls, notifications, data isolation, or runtime behavior. Separate application credentials, identity, packaging and Windows testing remain required.

## Experimental built-in Windows theme

A selectable Miugram theme is included in Settings → Chat settings, alongside the upstream themes. It uses a graphite and muted-pink palette based on the upstream night theme. Core contrast and archive structure are checked statically; the theme, settings layout and accent picker have not yet been compiled or visually tested. It is not yet selected by default on first launch.

## Experimental Windows desktop shortcut icon

Settings offers all four supplied icon concepts for a desktop shortcut. Selecting one creates Miugram.lnk if absent or changes it only if the existing shortcut targets this executable. Icon files are stored in Miugram’s local working directory; Windows Explorer may cache the old image. The executable, taskbar, Start menu and already pinned shortcuts are not changed by this setting. This code has not been compiled or tested on Windows.

The debug CI workflow reads the Telegram API ID and hash from GitHub Actions secrets, instead of upstream test credentials. This does not prove that login works or make the build a release. The API hash is embedded in the executable and is extractable from it.

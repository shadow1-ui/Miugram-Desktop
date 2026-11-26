# Miugram for Windows

Miugram is an unofficial Telegram client based on the official upstream source. The upstream README, license and third-party notices remain applicable.

## Baseline

- Upstream: https://github.com/telegramdesktop/tdesktop
- Initial commit: `1fdd0ea143accc8b5d917e95144cfc5f73e06395`
- Project assets in `miugram/assets/logos`: the four supplied concepts and original composite. Four multi-resolution Windows ICO resources have been generated from the supplied concepts; cat and paper plane is the default executable icon. The other three are not yet selectable.

## Current status

A first Miugram entry in Desktop settings displays the current account ID and current DC when available. It is unbuilt and untested on Windows. No tested Miugram builds or downloadable releases are available. The Windows executable, app-data name, shortcut name, notification identifiers and default executable icon have been changed for Miugram, and the upstream auto-updater is disabled by default. These changes are untested on Windows and do not establish complete data isolation. The installer, signing identity, API credentials, and remaining branding need work before distribution. Do not use the upstream installer to distribute Miugram. Never commit credentials or signing keys. Follow the upstream build instructions and supply your own Telegram API ID and hash.

## Planned features

A cohesive dark graphite and muted pink design; fast, consistent animations; a dedicated Miugram settings area; account ID and real current DC; user links with privacy limitations; configurable message status indicators; local profile background with cropping; and selectable app icons where supported. Custom interface icon design is deferred pending approval.

The requested option to retain locally received messages deleted by others must be clearly marked, safely stored and deletable. It cannot recover messages never received and must not bypass self-destructing or protected content.

The requested read-receipt suppression mode is not implemented. Telegram's API terms explicitly prohibit manipulating read statuses, including ghost mode; violation can result in loss of API access. Other sessions and queued actions can also affect read status. See https://core.telegram.org/api/terms.

## Build verification

The `Miugram Windows debug build` workflow attempts the upstream Windows dependency preparation and a debug compile with `TDESKTOP_API_TEST`. It publishes no executable and is not a release pipeline. A successful compile would not verify login, calls, notifications, data isolation, or runtime behavior. Separate application credentials, identity, packaging and Windows testing remain required.

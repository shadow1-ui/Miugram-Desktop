# Miugram for Windows

Miugram is an unofficial Telegram client based on the official upstream source. The upstream README, license and third-party notices remain applicable.

## Baseline

- Upstream: https://github.com/telegramdesktop/tdesktop
- Initial commit: `1fdd0ea143accc8b5d917e95144cfc5f73e06395`
- Project assets in `miugram/assets/logos`: the four supplied concepts and original composite. They are references, not yet production-ready icon resources; cat and paper plane is the intended default.

## Current status

A first Miugram entry in Desktop settings displays the current account ID and current DC when available. It is unbuilt and untested on Windows. No tested Miugram builds or downloadable releases are available. The existing Telegram application name, package/signing identity, API credentials, and branding still need to be replaced before distribution. Never commit credentials or signing keys. Follow the upstream build instructions and supply your own Telegram API ID and hash.

## Planned features

A cohesive dark graphite and muted pink design; fast, consistent animations; a dedicated Miugram settings area; account ID and real current DC; user links with privacy limitations; configurable message status indicators; local profile background with cropping; and selectable app icons where supported. Custom interface icon design is deferred pending approval.

The requested option to retain locally received messages deleted by others must be clearly marked, safely stored and deletable. It cannot recover messages never received and must not bypass self-destructing or protected content.

The requested read-receipt suppression mode is not implemented. Telegram's API terms explicitly prohibit manipulating read statuses, including ghost mode; violation can result in loss of API access. Other sessions and queued actions can also affect read status. See https://core.telegram.org/api/terms.

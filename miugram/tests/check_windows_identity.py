#!/usr/bin/env python3
"""Static guardrails, not a Windows runtime or installation test."""
from pathlib import Path
import struct

root = Path(__file__).resolve().parents[2]
def text(path):
    return (root / path).read_text(encoding='utf-8')
version = text('Telegram/SourceFiles/core/version.h')
model = text('Telegram/SourceFiles/platform/win/windows_app_user_model_id.cpp')
specific = text('Telegram/SourceFiles/platform/win/specific_win.cpp')
toast = text('Telegram/SourceFiles/platform/win/windows_toast_activator.h')
rc = text('Telegram/Resources/winrc/Telegram.rc')
cmake = text('Telegram/CMakeLists.txt')
assert 'AppName = "Miugram"' in version and 'AppFile = "Miugram"' in version
assert '{53F49750-6209-4FBF-9CA8-7A333C87D1ED}' not in version
assert 'Telegram.TelegramDesktop' not in model
assert 'Telegram.lnk' not in model and 'Telegram.lnk' not in specific
assert 'F11932D3-6110-4BBC-9B02-B2EC07A1BD19' not in toast
assert 'miugram-1.ico' in rc and 'set(output_name "Miugram")' in cmake
assert 'set(DESKTOP_APP_DISABLE_AUTOUPDATE ON CACHE BOOL' in text('CMakeLists.txt')
for i in range(1, 5):
    data = (root / f'Telegram/Resources/art/miugram-{i}.ico').read_bytes()
    reserved, kind, count = struct.unpack_from('<HHH', data)
    assert (reserved, kind) == (0, 1) and count >= 7
    sizes = set()
    for j in range(count):
        w, h, _, _, _, _, size, offset = struct.unpack_from('<BBBBHHII', data, 6 + j * 16)
        sizes.add((w or 256, h or 256))
        assert size > 0 and offset + size <= len(data)
    assert {(16,16),(32,32),(48,48),(256,256)} <= sizes
print('Windows identity and ICO static checks passed (runtime unverified).')

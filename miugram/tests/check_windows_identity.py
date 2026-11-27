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
# These are structural checks, not a replacement for testing Explorer/COM.
import xml.etree.ElementTree as ET
resource = ET.parse(root / 'Telegram/Resources/qrc/telegram/telegram.qrc')
aliases = {e.get('alias') for e in resource.findall('.//file')}
assert all(f'art/miugram-{i}.ico' in aliases for i in range(1, 5))
win = text('Telegram/SourceFiles/platform/win/specific_win.cpp')
settings = text('Telegram/SourceFiles/settings/sections/settings_main.cpp')
assert 'bool Platform::SetMiugramShortcutIcon(int icon)' in win
assert 'SLGP_RAWPATH' in win and 'Qt::CaseInsensitive' in win
assert 'SetIconLocation' in win and 'SHCNE_UPDATEITEM' in win
assert 'Platform::SetMiugramShortcutIcon(group->current())' in settings
print('Desktop shortcut icon integration statically checked (Windows runtime unverified).')

assert 'class MiugramSettings final : public Section<MiugramSettings>' in settings
assert '.targetSection = MiugramSettingsId()' in settings
assert 'BuildMiugramButtons(builder);' in settings
assert '.parentId = MainId()' in settings
print('Dedicated Miugram settings navigation statically checked (Windows runtime unverified).')

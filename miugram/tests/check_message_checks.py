from pathlib import Path
from struct import unpack
root = Path(__file__).resolve().parents[2]
icons = root / 'Telegram/Resources/icons'
for suffix, scale in (('', 1), ('@2x', 2), ('@3x', 3)):
    for name, width in (('sent', 13), ('received', 18)):
        data = (icons / f'miugram_{name}_round{suffix}.png').read_bytes()
        assert data[:8] == b'\x89PNG\r\n\x1a\n'
        assert unpack('>II', data[16:24]) == (width * scale, 11 * scale)
style = (root / 'Telegram/SourceFiles/ui/chat/chat_style.cpp').read_text()
for label in ('Sent', 'Received'):
    for ending in ('Icon', 'SelectedIcon', 'InvertedIcon'):
        assert f'miugram{label}Rounded{ending}' in style
bottom = (root / 'Telegram/SourceFiles/history/view/history_view_bottom_info.cpp').read_text()
assert 'Data::Flag::Sending' in bottom and ': unread' in bottom
assert 'historySentIcon' in bottom and 'historyReceivedIcon' in bottom
settings = (root / 'Telegram/SourceFiles/settings/sections/settings_main.cpp').read_text()
assert 'Miugram::SetCheckStyle(group->current())' in settings
print('Message check resources and integration statically checked (visual behavior unverified).')

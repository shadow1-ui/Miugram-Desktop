from pathlib import Path

root = Path(__file__).resolve().parents[2]
helper = (root / 'Telegram/SourceFiles/miugram/profile_background.h').read_text()
settings = (root / 'Telegram/SourceFiles/settings/sections/settings_main.cpp').read_text()
top = (root / 'Telegram/SourceFiles/info/profile/info_profile_top_bar.cpp').read_text()
strings = (root / 'Telegram/Resources/langs/lang.strings').read_text()
assert 'tdata/miugram/profile-%1.png' in helper
assert all(x in helper for x in ('QSaveFile', 'output.commit()', 'QFile::remove', '1600'))
assert all(x in settings for x in ('main/miugram_profile_background', 'PrepareProfilePhotoFromFile', 'SaveProfileBackground', 'RemoveProfileBackground', 'MakeMyProfile'))
assert '_peer->isSelf() && !_savedMessages' in top
assert 'LoadProfileBackground(_peer->session().userId().bare)' in top
assert all(('"lng_miugram_background_' + x + '"') in strings for x in ('choose', 'preview', 'remove', 'saved', 'removed', 'failed'))
print('Profile-background integration checks passed (not a compile or UI test).')

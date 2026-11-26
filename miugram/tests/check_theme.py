#!/usr/bin/env python3
"""Check theme archive, integration and core contrast; not a visual QA test."""
from pathlib import Path
import re, zipfile
root = Path(__file__).resolve().parents[2]
with zipfile.ZipFile(root / 'Telegram/Resources/miugram.tdesktop-theme') as z:
    assert z.testzip() is None
    assert set(z.namelist()) == {'colors.tdesktop-theme', 'background.png'}
    data = z.read('colors.tdesktop-theme').decode()
with zipfile.ZipFile(root / 'Telegram/Resources/night.tdesktop-theme') as z:
    original=z.read('colors.tdesktop-theme').decode()
def keys(s): return re.findall(r'^([A-Za-z0-9_]+):',s,re.M)
assert keys(data)==keys(original)
def color(k):
    seen=set()
    while True:
        assert k not in seen, 'cyclic color alias'
        seen.add(k)
        m=re.search(r'^'+re.escape(k)+r': ([A-Za-z0-9_#]+);',data,re.M);assert m,k
        value=m[1]
        if value.startswith('#'):
            assert len(value)==7, k
            return [int(value[i:i+2],16)/255 for i in (1,3,5)]
        k=value
def lum(c):
    return sum(w*(v/12.92 if v <= .04045 else ((v+.055)/1.055)**2.4) for w,v in zip((.2126,.7152,.0722),c))
def contrast(a,b):
    l1,l2=sorted((lum(color(a)),lum(color(b))),reverse=True);return (l1+.05)/(l2+.05)
for pair in [('windowFg','windowBg'),('windowSubTextFg','windowBg'),('windowFgActive','windowBgActive'),('activeButtonFg','activeButtonBg'),('lightButtonFg','lightButtonBg'),('dialogsNameFg','dialogsBg'),('dialogsTextFg','dialogsBg'),('dialogsDateFg','dialogsBg'),('dialogsUnreadFg','dialogsUnreadBg'),('historyTextInFg','msgInBg'),('historyTextOutFg','msgOutBg')]:
    c=contrast(*pair);print(*pair,round(c,2));assert c>=4.5
assert 'miugram.tdesktop-theme' in (root/'Telegram/Resources/qrc/telegram/telegram.qrc').read_text()
assert 'EmbeddedType::Miugram' in (root/'Telegram/SourceFiles/window/themes/window_themes_embedded.cpp').read_text()
print('Theme archive and core contrast checks passed; visual testing still required.')

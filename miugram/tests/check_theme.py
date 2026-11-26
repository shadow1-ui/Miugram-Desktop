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
    m=re.search(r'^'+k+r': #([0-9a-f]{6});',data,re.M);assert m,k
    return [int(m[1][i:i+2],16)/255 for i in (0,2,4)]
def lum(c):
    return sum(w*(v/12.92 if v <= .04045 else ((v+.055)/1.055)**2.4) for w,v in zip((.2126,.7152,.0722),c))
def contrast(a,b):
    l1,l2=sorted((lum(color(a)),lum(color(b))),reverse=True);return (l1+.05)/(l2+.05)
for pair in [('windowFg','windowBg'),('windowSubTextFg','windowBg'),('windowFgActive','windowBgActive'),('activeButtonFg','activeButtonBg'),('lightButtonFg','lightButtonBg')]:
    c=contrast(*pair);print(*pair,round(c,2));assert c>=4.5
assert 'miugram.tdesktop-theme' in (root/'Telegram/Resources/qrc/telegram/telegram.qrc').read_text()
assert 'EmbeddedType::Miugram' in (root/'Telegram/SourceFiles/window/themes/window_themes_embedded.cpp').read_text()
print('Theme archive and core contrast checks passed; visual testing still required.')

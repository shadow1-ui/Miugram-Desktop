from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'Telegram/Resources/icons'
for name, width, paths in (
    ('miugram_sent_round', 13, [[(1.5, 5.5), (4.4, 8.4), (11.2, 1.7)]]),
    ('miugram_received_round', 18, [[(1.3, 5.7), (4.1, 8.5), (10.8, 1.8)], [(6.1, 5.7), (9., 8.5), (15.7, 1.8)]]),
):
    for scale in (1, 2, 3):
        supersample = scale * 8
        image = Image.new('L', (width * supersample, 11 * supersample), 0)
        draw = ImageDraw.Draw(image)
        radius = 1.05 * supersample
        for path in paths:
            points = [(round(x * supersample), round(y * supersample)) for x, y in path]
            draw.line(points, fill=255, width=round(2.1 * supersample), joint='curve')
            for x, y in points:
                draw.ellipse((round(x-radius), round(y-radius), round(x+radius), round(y+radius)), fill=255)
        image = image.resize((width * scale, 11 * scale), Image.Resampling.LANCZOS).convert('RGB')
        suffix = '' if scale == 1 else f'@{scale}x'
        image.save(root / f'{name}{suffix}.png')

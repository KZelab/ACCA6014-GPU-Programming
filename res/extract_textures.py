#!/usr/bin/env python3
"""Create texture atlas from Kenney Voxel Pack (CC0 licensed)"""

import os
from PIL import Image

# Configuration
TILES_DIR = r"C:\Users\natha\dev\CPP\GPUProgramming\Renderer\res\kenney_voxel-pack\PNG\Tiles"
TEXTURE_SIZE = 16  # Resize from 128x128 to 16x16
ATLAS_SIZE = 256

# Texture mapping (filename -> atlas index)
TEXTURE_MAP = {
    'air': 0, 'greystone': 1, 'dirt': 2, 'grass_top': 3, 'dirt_grass': 4,
    'sand': 5, 'water': 6, 'trunk_top': 7, 'trunk_side': 8, 'leaves': 9,
    'glass': 10, 'stone': 11, 'wood': 12, 'rock': 13, 'gravel_stone': 14,
    'brick_red': 15, 'snow': 16, 'ice': 17,
}

if __name__ == '__main__':
    atlas = Image.new('RGBA', (ATLAS_SIZE, ATLAS_SIZE), (0, 0, 0, 0))
    textures_per_row = ATLAS_SIZE // TEXTURE_SIZE

    for name, index in TEXTURE_MAP.items():
        if name == 'air':
            continue

        path = os.path.join(TILES_DIR, f'{name}.png')
        if not os.path.exists(path):
            print(f"Missing: {name}")
            continue

        texture = Image.open(path).convert('RGBA').resize((TEXTURE_SIZE, TEXTURE_SIZE), Image.LANCZOS)
        row, col = index // textures_per_row, index % textures_per_row
        atlas.paste(texture, (col * TEXTURE_SIZE, row * TEXTURE_SIZE))
        print(f"Added {name} at index {index}")

    atlas.save('kenney_voxel_atlas.png')
    print(f"\nAtlas saved: kenney_voxel_atlas.png ({ATLAS_SIZE}x{ATLAS_SIZE})")

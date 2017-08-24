#!/usr/bin/env python3
from PIL import Image
from os.path import splitext, basename
from glob import glob
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

images = []

images += [ { 'scale_factor': 0.7, 'file_name': f } for f in glob('../gfx/*alien*') ]
images += [ { 'scale_factor': 0.7, 'file_name': f } for f in glob('../gfx/*astronaut*') ]
images += [ { 'scale_factor': 0.5, 'file_name': f } for f in glob('../gfx/*Tile*') ]

tilemap_temp = Image.new('RGBA', (5000, 100), (255, 0, 0, 0))

curr_x = 0
max_y = 0

sprites = []

for image in images:
    im = Image.open(image['file_name'])
    scaled_im = im.resize((round(im.width * image['scale_factor']), round(im.height * image['scale_factor'])), Image.LANCZOS)
    tilemap_temp.paste(scaled_im, (curr_x, 0))
    sprites.append({ 'name': splitext(basename(image['file_name']))[0], 'posX': curr_x,
                     'width': scaled_im.width, 'height': scaled_im.height })
    curr_x += scaled_im.width
    max_y = max(scaled_im.height, max_y)

tilemap = tilemap_temp.crop((0, 0, curr_x, max_y))
tilemap.save('../dist/assets/tileset.png', optimize=True)

with open('../tileset.hpp', 'w') as f:
    f.write('''// generated by tools/tileset_gen.py

#pragma once

#include <SFML/System.hpp>

#include "spriteinfo.hpp"

namespace Tileset {

''')
    for sprite in sprites:
        f.write('const SpriteInfo %s = { %s, %s, { sf::Vector2f(%s, %s), sf::Vector2f(%s, %s), sf::Vector2f(%s, %s), sf::Vector2f(%s, %s) } };\n' % (
                sprite['name'],
                sprite['width'], sprite['height'],
                sprite['posX'], 0,
                sprite['posX'] + sprite['width'], 0,
                sprite['posX'] + sprite['width'], sprite['height'],
                sprite['posX'], sprite['height']
                ))
    f.write('\n}\n')

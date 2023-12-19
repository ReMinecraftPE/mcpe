#!/usr/bin/python3

# Minecraft PE Reverse Engineering Project
# Copyright (C) 2023 iProgramInCpp
# -----------------------------------------------
# This tool grabs the sound data from the .so file from the source Minecraft PE
# Android *.so file.  Offsets are hardcoded to that version, nothing else will work.
#
# TODO: Lookup ELF symbols to un-hardcode the offsets?
import sys
import os
import hashlib

# use memory efficient range always
merange = range
try:
    merange = xrange # python 2
except NameError:
    pass

# defines
pcm_addresses = [
    ('PCM_cloth1',     0x00112B08),
    ('PCM_cloth2',     0x00117134),
    ('PCM_cloth3',     0x0011B760),
    ('PCM_cloth4',     0x0011FD8C),
    ('PCM_grass1',     0x001243B8),
    ('PCM_grass2',     0x0012F00C),
    ('PCM_grass3',     0x00139C60),
    ('PCM_grass4',     0x001448B4),
    ('PCM_gravel1',    0x0014F508),
    ('PCM_gravel2',    0x0015A15C),
    ('PCM_gravel3',    0x00164DB0),
    ('PCM_gravel4',    0x0016FA04),
    ('PCM_sand1',      0x0017A658),
    ('PCM_sand2',      0x00181214),
    ('PCM_sand3',      0x00187DD0),
    ('PCM_sand4',      0x0018E98C),
    ('PCM_stone1',     0x00195548),
    ('PCM_stone2',     0x001A019C),
    ('PCM_stone3',     0x001AADF0),
    ('PCM_stone4',     0x001B5A44),
    ('PCM_wood1',      0x001C0698),
    ('PCM_wood2',      0x001CB2EC),
    ('PCM_wood3',      0x001D5F40),
    ('PCM_wood4',      0x001E0B94),
    ('PCM_click',      0x001EB7E8),
    ('PCM_explode',    0x001F78A8),
    ('PCM_splash',     0x00209A38)
]

so_file_sha256 = '157af341d13a54cc935bbe24c5e1cf3d02d7e40ec20f9859b9853c2e996ebd81'

header_1 = '// Minecraft Pocket Edition Reverse Engineering Project\n// Copyright (C) 2011 Mojang Specifications.\n'
header_2 = '// Data extracted from libminecraftpe.so (Minecraft PE v0.1.3.apk).\n\n'

def read_int_from_bytes(arr_bytes, offset):
    return arr_bytes[offset] | arr_bytes[offset + 1] << 8 | arr_bytes[offset + 2] << 16 | arr_bytes[offset + 3] << 24

def read_short_from_bytes(arr_bytes, offset):
    return arr_bytes[offset] | arr_bytes[offset + 1] << 8

def format_hex_short(short):
    return "{:04x}".format(short)

def main():
    if len(sys.argv) < 2:
        print('Usage:', sys.argv[0], '[libminecraftpe.so] (output dir)')
        return
    
    output_dir = ''
    
    if len(sys.argv) > 2:
        output_dir = sys.argv[2]
    
    if len(output_dir) == 0:
        output_dir = './sound_data/'
        
    # append a slash to the file name, if it doesn't have one already
    if output_dir[len(output_dir) - 1] != '/':
        output_dir += '/'
    
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    bytes = []
    
    # open the file
    try:
        with open(sys.argv[1],mode='rb') as sofile:
            # read all bytes
            bytes = sofile.read()
            
    except FileNotFoundError:
        print('ERROR: The file', sys.argv[1], 'was not found.')
        return
        
    # first off, hash the file to ensure it matches
    src_sha256 = hashlib.sha256(bytes).hexdigest()
    
    print('* Source file hash: ', src_sha256)
    
    if src_sha256 != so_file_sha256:
        print('ERROR: Source file does not match the Source APK\'s SO file. Please extract one from the aforementioned APK. See the readme for more details.')
        return
    
    print('* Extracting PCM data...')
    
    include_all_str = '#pragma once\n\n' + header_1
    include_all_str+= '\n// @NOTE: This is meant to be included in exactly 1 compile object/source file.\n\n' + header_2
    
    for item in pcm_addresses:
        include_all_str += '#include "' + item[0] + '.h"\n'
        ostr = header_1 + header_2
        
        channels = read_int_from_bytes(bytes, item[1] + 0)
        data_length = read_int_from_bytes(bytes, item[1] + 12)
        
        # @NOTE: All the PCM sound data is in `.data`. So there's no consts to be found.
        
        ostr += 'PCMSoundHeader '+item[0]+' =\n{\n'
        ostr += '\t' + str(channels) + ',\n'
        ostr += '\t' + str(read_int_from_bytes(bytes, item[1] + 4)) + ',\n'
        ostr += '\t' + str(read_int_from_bytes(bytes, item[1] + 8)) + ',\n'
        ostr += '\t' + str(data_length)
        ostr += '\n};\n\n'

        data_length *= channels
        
        ostr += 'uint16_t '+item[0]+'_data['+str(data_length)+'] =\n{'
        
        for i in merange(data_length):
            if i % 16 == 0:
                ostr += '\n\t'
            ostr += '0x'+format_hex_short(read_short_from_bytes(bytes, item[1] + 16 + i * 2)) + ','
        
        ostr += '\n};\n'
        
        filename = output_dir + item[0] + '.h'
        with open(filename, 'w') as of:
            of.write(ostr)
    
    include_all_str += '\n\n'
    
    for item in pcm_addresses:
        include_all_str += 'SoundDesc SA_' + item[0][4:] + '(' + item[0] + ', ' + item[0] + '_data);\n'
    
    with open(output_dir + 'sounds.h', 'w') as of:
        of.write(include_all_str)
    
    pass
    
    
if __name__ == '__main__':
    main()

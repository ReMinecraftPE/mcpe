# Minecraft PE Reverse Engineering Project
# Copyright (C) 2023 iProgramInCpp
# -----------------------------------------------
# This tool grabs the sound data from the .so file from the mcpe01_canada.apk. Offsets are hardcoded to that version, nothing else will work.
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
    ('cloth1',     0x0014CFE8),
    ('cloth2',     0x00151614),
    ('cloth3',     0x00155C40),
    ('cloth4',     0x0015A26C),
    ('grass1',     0x0015E898),
    ('grass2',     0x001694EC),
    ('grass3',     0x00174140),
    ('grass4',     0x0017ED94),
    ('gravel1',    0x001899E8),
    ('gravel2',    0x0019463C),
    ('gravel3',    0x0019F290),
    ('gravel4',    0x001A9EE4),
    ('sand1',      0x001B4B38),
    ('sand2',      0x001BB6F4),
    ('sand3',      0x001C22B0),
    ('sand4',      0x001C8E6C),
    ('stone1',     0x001CFA28),
    ('stone2',     0x001DA67C),
    ('stone3',     0x001E52D0),
    ('stone4',     0x001EFF24),
    ('wood1',      0x001FAB78),
    ('wood2',      0x002057CC),
    ('wood3',      0x00210420),
    ('wood4',      0x0021B074),
    ('click',      0x00225CC8),
    ('explode',    0x00231D88),
    ('splash',     0x00243F18)
]

so_file_sha256 = '74b1d3b51a174c4dcaaa58d2bbdf7a4f4c9cfe5f2aeec4844427c2b8f1124343'

#header_1 = '// Minecraft Pocket Edition Reverse Engineering Project\n// Copyright (C) 2011 Mojang Specifications.\n'
#header_2 = '// Data extracted from libminecraftpe.so (mcpe01_canada.apk).\n\n'

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
        output_dir = './sound_data_pcm/'
        
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
        print('ERROR: Source file does not match mcpe01_canada.apk\'s SO file. Please extract one from the aforementioned APK.')
        return
    
    print('* Extracting PCM data...')
    
    #include_all_str = '#pragma once\n\n' + header_1
    #include_all_str+= '\n// @NOTE: This is meant to be included in exactly 1 compile object/source file.\n\n' + header_2
    
    for item in pcm_addresses:
        #include_all_str += '#include "' + item[0] + '.h"\n'
        
        filename = output_dir + item[0] + '.pcm'
        with open(filename, 'wb') as of:
            data_length = read_int_from_bytes(bytes, item[1] + 12)
        
            # @NOTE: All the PCM sound data is in `.data`. So there's no consts to be found.
        
            # header - four integers
            of.write(read_int_from_bytes(bytes, item[1] + 0).to_bytes(4, byteorder='little'))
            of.write(read_int_from_bytes(bytes, item[1] + 4).to_bytes(4, byteorder='little'))
            of.write(read_int_from_bytes(bytes, item[1] + 8).to_bytes(4, byteorder='little'))
            of.write(data_length.to_bytes(4, byteorder='little'))
            
            # data - 16 bit samples
            for i in merange(data_length):
                of.write(read_short_from_bytes(bytes, item[1] + 16 + i * 2).to_bytes(2, byteorder='little'))
                pass
        pass
    
    pass
    
    
if __name__ == '__main__':
    main()

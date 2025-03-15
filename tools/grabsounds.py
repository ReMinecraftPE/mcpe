#!/usr/bin/env python3
import setup_venv

import lief
import sys
import os

# Main Function
def main(file):
    # Output Directory
    out = os.path.dirname(os.path.abspath(__file__))
    out = os.path.join(out, '..', 'game', 'assets', 'sound')
    os.makedirs(out, exist_ok=True)
    # Extract Sound Data
    elf = lief.parse(file)
    prefix = 'PCM_'
    for sym in elf.dynamic_symbols:
        if sym.name.startswith('PCM_'):
            # Found Sound Symbol, Extract It
            name = sym.name[len(prefix):]
            print(f'Extracting Sound {name}...')
            path = os.path.join(out, name + '.pcm')
            data = bytes(elf.get_content_from_virtual_address(sym.value, sym.size))
            with open(path, 'wb') as file:
                file.write(data)

# Entrypoint
if __name__ == '__main__':
    # Check Arguments
    if len(sys.argv) < 2:
        print(f'USAGE: {__file__} libminecraftpe.so')
        sys.exit()
    # Run
    main(sys.argv[1])
    # Done
    print('Done!')

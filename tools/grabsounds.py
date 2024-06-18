import setup_venv
import lief
import sys
import os

# Check Arguments
if len(sys.argv) < 2:
    print(f'Usage: {__file__} libminecraftpe.so')
    sys.exit()

# Output Directory
out = os.path.dirname(os.path.abspath(__file__))
out = os.path.join(out, '..', 'game', 'assets', 'sound')
os.makedirs(out, exist_ok=True)

# Extract Sound Data
elf = lief.parse(sys.argv[1])
prefix = 'PCM_'
for sym in elf.dynamic_symbols:
    if sym.name.startswith('PCM_'):
        name = sym.name[len(prefix):]
        print(f'Extracting {name}...')
        path = os.path.join(out, name + '.pcm')
        data = bytes(elf.get_content_from_virtual_address(sym.value, sym.size))
        with open(path, 'wb') as file:
            file.write(data)

# Done
print('Done!')

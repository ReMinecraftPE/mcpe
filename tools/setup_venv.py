import os
import venv
import sys
import subprocess
import platform

# Find Binary Directory
bin_dir = 'bin'
if platform.system() == 'Windows':
    bin_dir = 'Scripts'

# Paths
path = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(path, 'venv')
valid_file = os.path.join(path, '.valid')

# Open In Virtualenv
def restart_in_venv(path):
    print('Activating Virtualenv...')
    py = os.path.basename(sys.executable)
    py = os.path.join(path, bin_dir, py)
    os.execv(py, [py] + sys.argv)

# Create Virtualenv
def create_venv(path):
    # Check If It Already Exists
    if os.path.exists(valid_file):
        return
    # Build Environment
    print('Creating Virtualenv...')
    builder = venv.EnvBuilder(clear=True, symlinks=False, with_pip=True)
    builder.create(path)
    # Install LIEF
    pip = 'pip'
    if platform.system() == 'Windows':
        pip += '.exe'
    subprocess.run([os.path.join(path, bin_dir, pip), 'install', 'lief'])
    # Mark As Created
    open(valid_file, 'wb').close()

# Run
if not sys.executable.startswith(path):
    create_venv(path)
    restart_in_venv(path)

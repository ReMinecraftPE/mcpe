import os
import venv
import sys
import subprocess

# Open In Virtualenv
path = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(path, 'venv')
def restart_in_venv(path):
    print('Activating Virtualenv...')
    py = os.path.join(path, 'bin', 'python3')
    os.execv(py, [py] + sys.argv)

# Create Virtualenv
def create_venv(path):
    if os.path.exists(path):
        return
    print('Creating Virtualenv...')
    builder = venv.EnvBuilder(clear=True, symlinks=True, with_pip=True)
    builder.create(path)
    subprocess.run([os.path.join(path, 'bin', 'pip3'), 'install', 'lief'])

# Run
if not sys.executable.startswith(path):
    create_venv(path)
    restart_in_venv(path)

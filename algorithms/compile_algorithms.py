import os
import subprocess
import sys
import shutil

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
BIN_DIR = os.path.join(BASE_DIR, 'bin')
os.makedirs(BIN_DIR, exist_ok=True)

# Find compiler: check gcc first, then tools/tcc/tcc.exe
COMPILER = None
if shutil.which("gcc"):
    COMPILER = "gcc"
else:
    tcc_path = os.path.abspath(os.path.join(BASE_DIR, '..', 'tools', 'tcc', 'tcc.exe'))
    if os.path.exists(tcc_path):
        COMPILER = tcc_path

if not COMPILER:
    print("Error: No C compiler found (gcc or tcc)!")
    sys.exit(1)

print(f"Using C Compiler: {COMPILER}")

SOURCE_FILES = [
    'linked_list.c',
    'queue.c',
    'stack.c',
    'priority_queue.c',
    'merge_sort.c',
    'binary_search.c'
]

compiled_count = 0
for src in SOURCE_FILES:
    src_path = os.path.join(BASE_DIR, src)
    out_name = os.path.splitext(src)[0] + ('.exe' if os.name == 'nt' else '')
    out_path = os.path.join(BIN_DIR, out_name)
    
    cmd = [COMPILER, src_path, '-o', out_path]
    print(f"Compiling {src} -> {out_name}...")
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode == 0:
        print(f"  [SUCCESS] {out_name} created.")
        compiled_count += 1
    else:
        print(f"  [ERROR] Failed to compile {src}:")
        print(res.stderr)

print(f"\nCompilation finished: {compiled_count}/{len(SOURCE_FILES)} C binaries compiled successfully.")


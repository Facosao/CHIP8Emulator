import os
import sys
import fnmatch

dry_run = False

if len(sys.argv) == 2:
    if sys.argv[1] == "--dry-run":
        dry_run = True

def find(pattern, path):
    result = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result

#print(find('*.c', './'))
source_files = find('*.c', './')

cc = "gcc "
cflags = "-O3 -lSDL2 -lm -Wall -Wextra"

output = cc + cflags

for file in source_files:
    output += " " + file

output += " -o ./bin/chip8.out"

os.system("echo " + output)

if dry_run is False:
    os.system(output)

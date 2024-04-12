
import sys
import subprocess
import os

args = sys.argv

if len(args) < 5:
    print(" Usage: frame_copy.py <filename> <start frame num> <end frame num> <dest dir>")
    exit(0)

cwd = os.getcwd()

fn = args[1]
fullfilepath = os.path.join(cwd, fn)

start = int(args[2])
end = int(args[3])
destdir = args[4]

fulldestpath = os.path.join(cwd, destdir)

for num in range(start, end+1):
    destfile = os.path.join(fulldestpath, f"{num}.png")
    cmd = f"cp {fullfilepath} {destfile}"
    print(cmd)

    os.system(cmd)
    



import sys
import os

args = sys.argv

if len(args) < 4:
    print(" Usage: scale.py <srcr> <dest> <end>")
    exit(0)

cwd = os.getcwd()
srcfolder = args[1]
fullsrcpath = os.path.join(cwd, srcfolder)

destfolder = args[2]
fulldestpath = os.path.join(cwd, destfolder)

end = int(args[3])

for srcfile in os.listdir(fullsrcpath):
    filenum = int(srcfile.split(".")[0])
    srcfilepath = os.path.join(fullsrcpath, srcfile)
    destfilepath = os.path.join(fulldestpath, f"{end-filenum}.png")

    cmd = f"cp {srcfilepath} {destfilepath}"
    print(cmd)

    os.system(cmd)


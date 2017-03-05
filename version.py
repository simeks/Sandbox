import sys
import os
import subprocess
import shutil

def get_commit_id():
    proc = subprocess.Popen(["git","rev-parse", "--short","HEAD"], stdout=subprocess.PIPE)
    id = proc.stdout.readline().strip()
    return id.decode("utf-8")

def write_version_header(file):
    commit_id = get_commit_id()
    with open(file, 'w') as f:
        f.write("#ifndef __GENERATED_ENGINE_VERSION_H__\n"
                "#define __GENERATED_ENGINE_VERSION_H__\n"
                "\n"
                "#define ENGINE_COMMIT_ID " + commit_id + "\n"
                "\n"
                "#endif // __GENERATED_ENGINE_VERSION_H__\n")

def main(argv):
    if len(argv) >= 2:
        write_version_header(argv[1])

if __name__ == "__main__":
    main(sys.argv)
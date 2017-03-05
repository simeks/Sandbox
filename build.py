#bin/python

import sys
import os
import subprocess
import shutil

builder_exec = "Binaries/Win64/Builder-release.exe"


def copy_dep(target_path):
    if (os.path.isdir("External")):
        f = open("External/shared_libraries.txt")
        dest = ""
        for line in f:
            line = line.strip()
            if line == "[32]":
                dest = target_path+"Win32"
            elif line == "[64]":
                dest = target_path+"Win64"
            elif line != "" and dest != "":
                shutil.copy2("External/"+line, dest)
    

def setup_directories(base_dir = "Binaries/"):
    if (os.path.isdir(base_dir) == False):
        os.mkdir(base_dir)
    if (os.path.isdir(base_dir+"Win32") == False):
        os.mkdir(base_dir+"Win32")
    if (os.path.isdir(base_dir+"Win64") == False):
        os.mkdir(base_dir+"/Win64")
    if (os.path.isdir(base_dir+"Content") == False):
        os.mkdir(base_dir+"Content")
    copy_dep(base_dir)
    

def run_builder(content_source, content_target, server_mode = True):
    setup_directories()
    if (subprocess.call(["tundra2", "Builder", "release"]) != 0):
        print("Could not run builder")
        return
    cmd = [builder_exec,"--source",content_source,"--target",content_target]
    if server_mode:
        cmd.append("--server")
        subprocess.Popen(cmd)
    else:
        subprocess.call(cmd)


def get_commit_id():
    proc = subprocess.Popen(["git","rev-parse", "--short","HEAD"], stdout=subprocess.PIPE)
    id = proc.stdout.readline().strip()
    return id.decode("utf-8")

def build_program(target, platform, version, clean_build = False):
    arg = ""
    if clean_build:
        arg = "-l"
    cmd = ["tundra2", arg, target, platform, version]
    return subprocess.call(cmd)

def run_tests():
    print("\n===== Testing Win64 =====")
    ret = subprocess.call("Binaries/Win64/Test_Foundation-release.exe")
    if ret != 0:
        return ret
    print("\n===== Testing Win32 =====")
    ret = subprocess.call("Binaries/Win32/Test_Foundation-release.exe")
    return ret
    

def build_release():
    if (os.path.isdir("Release") == False):
        os.mkdir("Release")

    commit_id = get_commit_id()
    release_name = "Sandbox-"+commit_id

    target_path = "Release"+"/"+release_name+"/"
    if (os.path.isdir(target_path) == True):
        shutil.rmtree(target_path)
    
    setup_directories(target_path)
    
    # Clean build of the runnables
    if build_program("Launcher", "win64-vs2013", "release", True) != 0:
        shutil.rmtree(target_path) # Cleanup
        return
    if build_program("Builder", "win64-vs2013", "release", False) != 0:
        shutil.rmtree(target_path) # Cleanup
        return
    if build_program("Launcher", "win32-vs2013", "release", True) != 0:
        shutil.rmtree(target_path) # Cleanup
        return
    if build_program("Builder", "win32-vs2013", "release", False) != 0:
        shutil.rmtree(target_path) # Cleanup
        return

    # Tests
    if build_program("Test_Foundation", "win64-vs2013", "release") != 0:
        shutil.rmtree(target_path) # Cleanup
        return
    if build_program("Test_Foundation", "win32-vs2013", "release") != 0:
        shutil.rmtree(target_path) # Cleanup
        return

    if run_tests() != 0:
        shutil.rmtree(target_path) # Cleanup
        return
    
    # Copy programs to target path
    shutil.copy2("Binaries/Win64/Launcher-release.exe", target_path+"Win64/Sandbox.exe")
    shutil.copy2("Binaries/Win64/Builder-release.exe", target_path+"Win64/Builder.exe")
    shutil.copy2("Binaries/Win32/Launcher-release.exe", target_path+"Win32/Sandbox.exe")
    shutil.copy2("Binaries/Win32/Builder-release.exe", target_path+"Win32/Builder.exe")

    # Build content
    run_builder("Content", target_path+"Content", False)
    
    print("Release '" + release_name + "' built.")

def main(argv):
    if len(argv) < 2:
        print("Usage: "+argv[0]+" <action>")
        return

    if argv[1] == "setup":
        setup_directories()
    elif argv[1] == "assets":
        run_builder("Content", "Binaries/Content", False)
    elif argv[1] == "release":
        build_release()

if __name__ == "__main__":
    main(sys.argv)

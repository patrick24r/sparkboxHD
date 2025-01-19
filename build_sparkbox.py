import argparse
import inspect
import os
import shutil
import subprocess

from sys import platform

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description='Build Sparkbox')
    parser.add_argument('device')
    parser.add_argument('-c', '--clean', action='store_true')
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

    # Check that the device folder exists at device/<device>
    # with proper toolchain file device/<device>/<device>.cmake
    device_dir = os.path.join(script_dir, "device", args.device)
    if not os.path.exists(device_dir):
        raise Exception("Device directory " + device_dir + " not found")
    toolchain_file = os.path.join(device_dir, args.device + ".cmake")
    if not os.path.exists(toolchain_file):
        raise Exception("Toolchain file " + toolchain_file + " not found")

    # Set up build directory
    build_dir = os.path.join(script_dir, "build", args.device)
    os.makedirs(build_dir, exist_ok=True)
    if args.clean:
        for root, dirs, files in os.walk(build_dir):
            for f in files:
                os.unlink(os.path.join(root, f))
            for d in dirs:
                shutil.rmtree(os.path.join(root, d))

    # Run cmake
    subprocess.run(["cmake", ".", "-B", build_dir, "-G", "Ninja", "-DCMAKE_TOOLCHAIN_FILE="+toolchain_file])
    # Build
    subprocess.run(["ninja", "-C", build_dir, "-v"])

    # Copy sound files from each level/<level>/sounds to build/<device>/<level>/sounds
    level_dir = os.path.join(script_dir, "level")
    for level_name in os.listdir(level_dir):
        level_name_dir = os.path.join(level_dir, level_name)
        if (os.path.isdir(level_name_dir)):
            build_level_dir = os.path.join(build_dir, "level", level_name, "sounds")
            if (os.path.exists(build_level_dir)):
                shutil.rmtree(build_level_dir)
            shutil.copytree(os.path.join(level_name_dir, "sounds"), build_level_dir)


# Using the special variable  
# __name__ 
if __name__=="__main__": 
    main()


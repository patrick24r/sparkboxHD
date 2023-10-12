import argparse
import os
import shutil
import subprocess

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description='Build Sparkbox')
    parser.add_argument('device')
    parser.add_argument('-c', '--clean', action='store_true')
    args = parser.parse_args()

    # Check that the device folder exists at device/<device>
    # with proper toolchain file device/<device>/<device>.cmake
    device_dir = os.path.join("device", args.device)
    if not os.path.exists(device_dir):
        raise Exception("Device directory " + device_dir + " not found")
    toolchain_file = os.path.join(device_dir, args.device + ".cmake")
    if not os.path.exists(toolchain_file):
        raise Exception("Toolchain file " + toolchain_file + " not found")

    # Set up build directory
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.mkdir(build_dir)
    if args.clean:
        for root, dirs, files in os.walk(build_dir):
            for f in files:
                os.unlink(os.path.join(root, f))
            for d in dirs:
                shutil.rmtree(os.path.join(root, d))

    # Run cmake
    subprocess.run(["cmake", ".", "-B", build_dir, "-DCMAKE_TOOLCHAIN_FILE="+toolchain_file])
    # Run make
    subprocess.run(["make", "-C", build_dir, "-j", "VERBOSE=1"])


# Using the special variable  
# __name__ 
if __name__=="__main__": 
    main()


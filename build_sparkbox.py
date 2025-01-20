import argparse
import glob
import inspect
import os
import platform
import shutil
import subprocess


def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description='Build Sparkbox')
    parser.add_argument('device')
    parser.add_argument('-c', '--clean', action='store_true')
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(
        inspect.getfile(inspect.currentframe())))

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
    result = subprocess.run(["cmake", ".", "-B", build_dir,
                            "-G", "Ninja", "-DCMAKE_TOOLCHAIN_FILE="+toolchain_file])
    result.check_returncode()
    # Build
    result = subprocess.run(["ninja", "-C", build_dir, "-v"])
    result.check_returncode()

    # Now that the build is done, create the 'out' directory and populate it with assets
    out_dir = os.path.join(build_dir, "out")
    if (args.clean and os.path.exists(out_dir)):
        shutil.rmtree(out_dir)
    os.makedirs(out_dir, exist_ok=True)

    # Copy the app executable to out_dir
    shutil.copy(os.path.join(build_dir, "device",
                "app", "device.app"), out_dir)

    # Copy the sparkbox and sparkbox_os libs to out_dir/sparkbox
    spark_out_dir = os.path.join(out_dir, "sparkbox")
    os.makedirs(spark_out_dir, exist_ok=True)
    spark_build_dir = os.path.join(build_dir, "lib", "sparkbox")

    # Copy the sparkbox lib file to out/sparkbox/
    shutil.copy(find_lib_file(spark_build_dir), spark_out_dir)

    # Copy the os level
    copy_level_to_out_dir(os.path.join(
        spark_build_dir, "level", "os"), os.path.join(spark_out_dir, "os"))


def find_lib_file(directory: str) -> str:
    if (not os.path.exists(directory)):
        raise Exception("Directory '" + directory + "' does not exist")

    # Try to match .so file
    match_glob = os.path.join(directory, "*.so")
    for match in glob.glob(match_glob):
        return match

    # Try to match .dll file
    match_glob = os.path.join(directory, "*.dll")
    for match in glob.glob(match_glob):
        return match

    raise Exception("No library file found for glob '" +
                    match_glob + "*.{so,dll}'")


# Copy the level within the level_dir to out_dir, creating a new out_dir
def copy_level_to_out_dir(level_dir: str, out_dir: str):
    # level_dir: directory with the level's compiled library and two
    # out_dir: directory to copy assets and library
    #
    # out_dir
    # |-- level_lib.*
    # |-- sounds
    # |   |-- sound1.wav
    # |   |-- sound2.wav
    # |-- sprites
    #     |-- sprite1.spr

    # Remove the existing level dir if it exists to sync any new/deleted assets
    if (os.path.exists(out_dir)):
        shutil.rmtree(out_dir)
    os.makedirs(out_dir, exist_ok=True)

    # Copy the library file
    shutil.copy(find_lib_file(level_dir), out_dir)

    # Copy the assets from "sounds" and "sprites" if they exist
    sounds_src_dir = os.path.join(level_dir, "sounds")
    if (os.path.exists(sounds_src_dir)):
        sounds_dest_dir = os.path.join(out_dir, "sounds")
        os.makedirs(sounds_dest_dir, exist_ok=True)
        shutil.copytree(sounds_src_dir, sounds_dest_dir)

    sprites_src_dir = os.path.join(level_dir, "sprites")
    if (os.path.exists(sprites_src_dir)):
        sprites_dest_dir = os.path.join(out_dir, "sprites")
        os.makedirs(sprites_dest_dir, exist_ok=True)
        shutil.copytree(sprites_src_dir, sprites_dest_dir)


# Using the special variable
# __name__
if __name__ == "__main__":
    main()

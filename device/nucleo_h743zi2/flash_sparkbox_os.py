import inspect
import os
import subprocess

def main():
    #
    os_relative_dir = os.path.join("level", "os")
    os_relative_elf = os.path.join(os_relative_dir, "os.elf")
    os_relative_bin = os.path.join(os_relative_dir, "os.bin")

    # Search the build directory for the os elf file
    script_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

    os_absolute_elf = os.path.join(script_dir, "..", "..", "build", os_relative_elf)
    os_absolute_bin = os.path.join(script_dir, "..", "..", "build", os_relative_bin)

    if not os.path.exists(os_absolute_elf):
        raise Exception("os executable not found at " + os_absolute_elf)
    
    # Copy the elf to a bin
    subprocess.run(["arm-none-eabi-objcopy", "-O", "binary", os_absolute_elf, os_absolute_bin])

    # Flash the bin

# Using the special variable  
# __name__ 
if __name__=="__main__": 
    main()
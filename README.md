BSCOS (Blue Space Cat Operating System)

If you ever saw the project titled BSC-DOS that I made long ago, think of this as the spiritual sucessor to that piece of garbage.

This is a full bare-metal operating system that I specifically designed because I needed a USB bootable system that would boot instantly, while being able to be customised to give direct hardware info, mainly while repairing computers.

I am unlikely to maintin this very much because it does everything I need by default, there are some "dead" features in there, like the .BPK package manager being practically broken (and useless on a ramdisk or venoy USB)

You CANNOT run this in an IDE, due to the built in video drivers it requires acces to memory locations that are already full. Refer to the building section for infirnation on how to get an iso.

If you attempt to run the bscos.bin executable directly within your IDE, it will crash with a Segmentation Fault. The kernel is designed to run on bare metal; it attempts to access hardware memory (like 0xB8000), which the host OS will block for SOME REASON (way to be a party pooper).

This is a 32-bit x86 operating system designed for testing hardware (specifically tested on a Dell Dimension, and my laptop and VirtualBox. ) It is written in C and some NASM assembly.

If you attempt to run the bscos.bin executable directly within your host OS (Linux/CLion), it will crash with a Segmentation Fault. This is expected behavior. A kernel is designed to run on bare metal and attempts to access protected hardware memory (like 0xB8000), which the host OS will block.

To run the project, you must build the ISO and boot it using an emulator like QEMU or a Virtual Machine.
Prerequisites (Arch Linux)

The build system requires gcc-multilib for 32-bit compilation and several tools for ISO creation.


Building and Running:

    Prerequisites: nasm gcc-multilib grub libisoburn mtools xorriso qemu-desktop

    Build: run pack.py on kernel.c, it should give you a kernel.bin

    Configure CMakeLists.txt for the hardware you are booting.
    
    If you want to yknow, build the system add this to the bottum of CMakeLists:

    add_custom_command(TARGET bscos.bin POST_BUILD
    COMMAND python3 ${CMAKE_SOURCE_DIR}/pack.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running pack.py to generate ISO..."
    )

    Emulate it: qemu-system-i386 -cdrom bscos.iso run the 

    Boot it: The iso should already be bootable, just throw it on a ventoy drive.

Stuffs'n'stuff:

    boot.s: Assembly entry point. Handles the Multiboot header and sets up the stack before calling the C kernel.

    kernel.c: The main kernel logic. Includes a basic VGA text driver.

    linker.ld: Linker script used to align sections and define the 1MB load address.

    pack.py: Script for handling BPK package files.

Features:

    It boots!!!
    
    Very lightweight

    Easy to customize

    Ideal for custom hardware testing (why I made it)

    "claws" so in theory bash could be installed in here

    barely complient package manager.

Todo (help if you can!):

    Hard drive instead of ramdisk

    Package manager documentation

    Bugs

Thank you for supporting this project! I've spent months of my life creating this beautiful program,
If you enjoy it feel free to message me on discord @BlueSpaceCatno2 or if I ever am on my email BlueSpaceCatno2@outlook.com.

If you find any bug, really of any kind, open an issue, when I can I'll address it.

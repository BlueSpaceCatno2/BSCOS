BSCOS (Blue Space Cat Operating System)

Technical Warning: SIGSEGV (Exit Code 139)

If you attempt to run the bscos.bin executable directly within your host OS (Linux/CLion), it will crash with a Segmentation Fault. This is expected behavior. A kernel is designed to run on bare metal; it attempts to access protected hardware memory (like 0xB8000), which the host OS will block for security.

This is a 32-bit x86 operating system designed for BIOS-based hardware (specifically tested on a Dell Dimension) and VirtualBox. It is written in C and NASM assembly.

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

    Qemu: qemu-system-i386 -cdrom bscos.iso run the 

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
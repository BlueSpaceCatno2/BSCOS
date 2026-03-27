cmake_minimum_required(VERSION 3.20)
project(BSCOS C ASM_NASM)

#if I catch you compiling 64 bit again ill kill you
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")
set(CMAKE_ASM_NASM_FLAGS "-f elf32")

set(CMAKE_EXE_LINKER_FLAGS "-m32 -T ${CMAKE_SOURCE_DIR}/linker.ld -nostdlib")

add_executable(bscos.bin
        boot.s
        kernel.c
        claws.h
        claws.h
        claws.h
        claws.h
        # main.c < legacy kernel
)
#make the iso you bastard
add_custom_command(TARGET bscos.bin POST_BUILD
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso_root/boot/grub
        COMMAND cp ${CMAKE_BINARY_DIR}/bscos.bin ${CMAKE_BINARY_DIR}/iso_root/boot/
        COMMAND cp ${CMAKE_SOURCE_DIR}/grub.cfg ${CMAKE_BINARY_DIR}/iso_root/boot/grub/
        COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/bscos.iso ${CMAKE_BINARY_DIR}/iso_root
        COMMENT "ISO's done mate!"
)

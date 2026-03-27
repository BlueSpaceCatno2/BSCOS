//i686-elf-gcc -c kernel.c -o kernel.o -ffreestanding -O2
//legacy kernel for developer piece of mind.
//useless to the user.
/*

#include <stdint.h>
#include <stddef.h>

#define IDE_SAFE_MODE 1
#define COM1 0x3F8 //debugging over serial, rare but not hard to add

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void serial_print(const char* s) {
    for (int i = 0; s[i] != '\0'; i++) {
        outb(COM1, s[i]); //no, its very hard to add
    }
}

//dunno what vga is? is that some boomer slang?
uint16_t* const vga_buffer = (uint16_t*)0xB8000;
void k_print(const char* str, uint8_t color) {
    static int cursor = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            cursor = (cursor / 80 + 1) * 80;
        } else {
            vga_buffer[cursor++] = (uint16_t)str[i] | (uint16_t)color << 8;
        }
    }
}

int probe_hardware() {
    // IF YOU REMOVE THAT LINE IT WILL NOT WORK IN CLion!
    if (IDE_SAFE_MODE) return 0;
    return 1;
}

// "modules"
typedef struct {
    char name[16];
    void (*init)();
} KModule;

void init_graphics() { k_print("[OK] VESA Driver\n", 0x03); }
void init_input()    { k_print("[OK] PS/2 Mouse\n", 0x03); }

KModule driver_stack[] = {
    {"Graphics", init_graphics},
    {"Input", init_input}
};

//scawy
void k_main() {
    serial_print("BSCOS Kernel Booting...\n");

    if (!probe_hardware()) {
        k_print("!! SAFE MODE (IDE) ACTIVE !!\n", 0x04); // Red text
        k_print("Drivers disabled. Bypassing Module Registry.\n", 0x07);
        serial_print("Warning: Hardware probe failed or IDE Mode forced.\n");
    } else {
        k_print("BSCOS v5.1 - Standard Boot\n", 0x02); // Green text
        for (size_t i = 0; i < 2; i++) {
            driver_stack[i].init();
        }
    }

    k_print("\nReady", 0x0F);
    while(1) { __asm__ volatile("hlt"); }
}

*/
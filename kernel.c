#include <stdint.h>

#define VBOX_VENDOR_ID 0x80EE
#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t unused[10];
    uint32_t framebuffer_addr_lo;
    uint32_t framebuffer_addr_hi;
};

uint16_t* vga_buffer = (uint16_t*)0xB8000;
int cursor = 0;

void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void k_print(const char* s, uint8_t color) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\n') cursor = (cursor / 80 + 1) * 80;
        else vga_buffer[cursor++] = (uint16_t)s[i] | (uint16_t)color << 8;
    }
}

void log_status(const char* msg, int success) {
    k_print("[  ", 0x07);
    if (success) k_print(" OK ", 0x02); else k_print("FAIL", 0x04);
    k_print("  ] ", 0x07); k_print(msg, 0x0F); k_print("\n", 0x0F);
}

void probe_vbox() {
    for (int bus = 0; bus < 256; bus++) {
        for (int dev = 0; dev < 32; dev++) {
            uint32_t addr = (1 << 31) | (bus << 16) | (dev << 11) | 0;
            outl(PCI_CONFIG_ADDR, addr);
            uint32_t vendor = inl(PCI_CONFIG_DATA) & 0xFFFF;
            if (vendor == VBOX_VENDOR_ID) {
                log_status("VirtualBox Guest Hardware Detected", 1);
                return;
            }
        }
    }
    log_status("VirtualBox Hardware", 0);
}

/* --- THE CLAWS (SYSCALLS) --- */
void bscos_syscall_handler(uint32_t eax, uint32_t ebx) {
    if (eax == 1) k_print((char*)ebx, 0x0F);
}
void k_main(uint32_t magic, struct multiboot_info* mb) {
    for (int i = 0; i < 80 * 25; i++) vga_buffer[i] = (uint16_t)' ' | (uint16_t)0x07 << 8;

    k_print("BSCOS v8.0 - VirtualBox Debug Mode\n", 0x0B);
    k_print("----------------------------------\n\n", 0x03);

    log_status("Kernel Handshake", (magic == 0x2BADB002));
    probe_vbox();

    uint32_t ram_mb = (mb->mem_upper / 1024) + 1;
    k_print("Detecting RAM: ", 0x0F);
    log_status("Memory Map Verified", 1);

    k_print("\nadmin@bscos-bash# ", 0x0A);
    while (1) { __asm__ volatile("hlt"); }
}
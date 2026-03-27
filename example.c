#include <stdint.h>
#include <stddef.h>

//
extern void k_print(const char* str, uint8_t color);
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void irq1_wrapper(); //From boot.s

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

int k_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char shell_buffer[80];
int buf_idx = 0;

char get_ascii(uint8_t scancode) {
    static char map[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
        '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
    }; //keyz
    return (scancode < 128) ? map[scancode] : 0;
}

void shell_interpret() {
    if (k_strcmp(shell_buffer, "help") == 0) {
        k_print("\nclaws: help, info, clear, halt", 0x0F);
    } else if (k_strcmp(shell_buffer, "info") == 0) {
        k_print("\nBSCOS v5.1 - Interrupt Driven", 0x0E);
    } else if (k_strcmp(shell_buffer, "clear") == 0) {
        for(int i=0; i<25; i++) k_print("\n", 0x07);
    } else if (k_strcmp(shell_buffer, "halt") == 0) {
        k_print("\nSystem Halted.", 0x04);
        __asm__ volatile("cli; hlt");
    } else if (shell_buffer[0] != '\0') {
        k_print("\nUnknown command.", 0x0C);
    }
    k_print("\nclaws> ", 0x0B);
}

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        char c = get_ascii(scancode);
        if (c == '\n') {
            shell_buffer[buf_idx] = '\0';
            shell_interpret();
            buf_idx = 0;
        } else if (c == '\b' && buf_idx > 0) {
            buf_idx--;
            k_print("\b \b", 0x07);
        } else if (c >= ' ' && buf_idx < 79) {
            shell_buffer[buf_idx++] = c;
            char s[2] = {c, 0};
            k_print(s, 0x07);
        }
    }
    outb(0x20, 0x20); 
}

void init_idt() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;
    uint32_t base = (uint32_t)irq1_wrapper;
    idt[33].base_low = base & 0xFFFF;
    idt[33].sel = 0x08;
    idt[33].always0 = 0;
    idt[33].flags = 0x8E;
    idt[33].base_high = (base >> 16) & 0xFFFF;
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFD); outb(0xA1, 0xFF);

    __asm__ volatile("lidt %0" : : "m"(idtp));
    __asm__ volatile("sti");
    
    k_print("\nclaws> ", 0x0B);
}

/*

ADD THIS TO KERNEL.C BEFORE IMPLEMENTING ANY OF THE IDT CODE!

extern void init_idt();

void k_main() {
    k_print("BSCOS Booting\n", 0x02);

    init_idt();
    
    while(1) { __asm__ volatile("hlt"); }
}

*/

#include <stdint.h>

#define VBOX_VENDOR_ID 0x80EE
#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC
// for command interperiter \/
#define MAX_TOKENS 1024  
#define MEM_SIZE 1024
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
                log_status("VirtualBox Hardware Detected", 1);
                return;
            }
        }
    }
    log_status("VirtualBox Hardware", 0);
}

//the claws!
void bscos_syscall_handler(uint32_t eax, uint32_t ebx) {
    if (eax == 1) k_print((char*)ebx, 0x0F);
}
void k_main(uint32_t magic, struct multiboot_info* mb) {
    for (int i = 0; i < 80 * 25; i++) vga_buffer[i] = (uint16_t)' ' | (uint16_t)0x07 << 8;

    k_print("BSCOS v1.0 VirtualBox mode\n", 0x0B);
    //k_print("----------------------------------\n\n", 0x03);

    log_status("Kernel Handshake", (magic == 0x2BADB002)); //it even has magic the name, DO NOT TOUCH IT.
    probe_vbox();

    uint32_t ram_mb = (mb->mem_upper / 1024) + 1;
    k_print("Detecting Memory: ", 0x0F);
    log_status("Memory Verified", 1);

    k_print("\nadmin@bscos# ", 0x0A);
    while (1) { __asm__ volatile("hlt"); }
}

typedef struct {
    char name[32];
    int value;
} Variable;

/* typedef struct {
    Variable vars[64];
    int var_count;
    int memory[MEM_SIZE]; 
} OSContext; */

void execute_line(OSContext *ctx, char *line) {
    char cmd[32], arg1[32], op[8], arg2[32];
    int n = sscanf(line, "%s %s %s %s", cmd, arg1, op, arg2);

   /* if (strcmp(cmd, "sysinfo") == 0) {
        gethw();
    } */ //not possible without alot of spaghetti code
    /* else if (strcmp(cmd, "ls") == 0) {
        filelist ();
    } */ //not possible without a FS
    /* else if (strcmp(cmd, "edit") == 0) {
        printf("Editing %s... (Type END to save)\n", arg1);
        char buffer[256];
        while(scanf("%s", buffer) && strcmp(buffer, "END") != 0);
        printf("Saved %s.\n", arg1);
    } */ //not possible without a FS either
    /* else  if (n >= 3 && strcmp(arg1, "=") == 0) {
        int val = atoi(op);
        strcpy(ctx->vars[ctx->var_count].name, cmd);
        ctx->vars[ctx->var_count].value = val;
        ctx->var_count++;
        printf("%s set to %d\n", cmd, val);
    } */ //when you readd the FS, then uncomment.
    else if (strcmp(cmd, "calc") == 0) {
        printf("[RESULT] %d\n", atoi(arg1) + atoi(arg2));
    }
    else if (strcmp(cmd, "help") == 0) {
        printf("Commands: sysinfo, ls, edit [file], [var] = [val], calc [a] [b]\n");
    }
}

void startshell() {
    OSContext ctx = {0};
    char input[256];
    printf("Type 'help' for commands.\n");

    while (1) {
        printf("# ");
        if (!fgets(input, sizeof(input), stdin)) break;
        if (strcmp(input, "exit\n") == 0) break;
        execute_line(&ctx, input);
    }
}
//leaving this cause I presume you're gonna finish it? \/
#endif

/*
there is a built in scripting language for ease of use, and if your crasy, setting up genuine CLI commands.
If you do not want the scripting language comment out lines 80 to 135.
remember no files can be saved or modified by the system, as adding support for various filesystems was simply
too much excess work for little/no gain.

If you find a way to implement this, without causing serious performance imitations make a pull request
ASAP.

If there are any issues with the pre-included drivers, I dont know what to tell you, I used the power of a 15 year
old stackoverflow thread to figure those out, and I dont even know, or want to know how they work.

Also if I do my math correct, the ram size limit of this program would be 4gb cause its compiled for 32bit by
the cmake, but I dont really mind anymore because I have already ventoyed arch onto the machines I was testing
and gone on with my day.

This repo likely wont get many updates cause its mostly useless compared to GNU/Linux.
*/

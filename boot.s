; boot.s
section .multiboot
align 4
    dd 0x1BADB002              ; Magic
    dd 0x05                    ; Flags (Mem Info + Graphics)
    dd -(0x1BADB002 + 0x05)    ; Checksum

    ; Graphics table (for VirtualBox VBE)
    dd 0, 0, 0, 0, 0
    dd 0                       ; 0 = Linear Graphics Mode
    dd 1024                    ; Width
    dd 768                     ; Height
    dd 32                      ; Depth (32-bit color)

section .text
global _start
global idt_load
extern k_main
extern idtp

_start:
    mov esp, stack_top
    push ebx                   ; Multiboot info (contains the VBE pointer)
    push eax                   ; Magic
    call k_main
    cli
.hang: hlt
    jmp .hang

idt_load:
    lidt [idtp]
    ret

section .bss
align 16
stack_bottom: resb 16384
stack_top:
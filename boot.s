; boot.s
section .multiboot
align 4
    dd 0x1BADB002
    dd 0x05
    dd -(0x1BADB002 + 0x05)

    ; Graphics table (for VirtualBox VBE)
    dd 0, 0, 0, 0, 0
    dd 0                       ; 0 =Linear Graphics
    dd 1024
    dd 768
    dd 32

section .text
global _start
global idt_load
extern k_main
extern idtp

_start:
    mov esp, stack_top
    push ebx
    push eax
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

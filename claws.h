#ifndef CLAWS_H
#define CLAWS_H

#include <stdint.h>

//give the kids claws
#define CLAW_WRITE 1
#define CLAW_EXIT  60
static inline void claw_call(uint32_t id, const void* arg1) {
    __asm__ volatile (
        "int $0x80"
        :
        : "a"(id), "b"(arg1)
        : "memory"
    );
}


void print(const char* msg) {
    claw_call(CLAW_WRITE, msg);
}

void exit() {
    claw_call(CLAW_EXIT, 0);
}

#endif
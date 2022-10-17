#ifndef CHIP8_HEADER
#define CHIP8_HEADER

#include <stdbool.h>
#include <stdint.h>

struct CHIP8 {
    bool key_pressed;
    uint16_t PC;
    int16_t DT;
    int16_t ST;
    uint16_t call_stack[16];
    uint16_t stack_pointer;
    uint8_t V[16];
    uint16_t VI;
    uint8_t keys[16];
    uint8_t memory[4096];
    uint8_t display[2048];
};

//int power(int base, int exponent);
struct CHIP8 chip8_new(void);
void chip8_run(struct CHIP8* self);

#endif

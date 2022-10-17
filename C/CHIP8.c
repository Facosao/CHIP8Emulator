#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "./include/CHIP8.h"
#include "./include/Font.h"

/*
int power(int base, int exponent) {
    
    int result = 1;

	while (exponent > 0) {
		result *= base;
		exponent -= 1;
    }
		
	return result;
}
*/

struct CHIP8 chip8_new(void) {
    
    srand(time(NULL));

    struct CHIP8 new;

    new.PC = 0x200;
    new.stack_pointer = 0;
    new.DT = 0;
    new.ST = 0;
    new.VI = 0;

    for (int i = 0; i < 4096; i++) {
        new.call_stack[i % 16] = 0;
        new.V[i % 16] = 0;
        new.keys[i % 16] = 0;
        new.memory[i] = 0;
        new.display[i % 2048] = 0;
    }

    return new;
}

void chip8_run(struct CHIP8* self) {

    uint16_t instruction = self->memory[self->PC] >> 4; // HI 4 bits - 1st byte
    uint8_t x = self->memory[self->PC] & 0xF; // --------- LO 4 bits - 1st byte
    uint8_t y = self->memory[self->PC + 1] >> 4; // ------ HI 4 bits - 2nd byte
    uint8_t nibble = self->memory[self->PC + 1] & 0xF; //- LO 4 bits - 2nd byte

    // Only done for aesthetic reasons (e.g. 0x3000 instead of 3 or 0x3)
    instruction <<= 12;

    uint8_t byte = self->memory[self->PC + 1];
    uint16_t address = (x << 8) | self->memory[self->PC + 1];

    uint8_t V[16];
    for (int i = 0; i < 16; i++)
        V[i] = self->V[i];

    /*
    fprintf(stderr, "PC = 0x%x opcode = 0x%02x%02x\n", self->PC, self->memory[self->PC], byte);
    
    fprintf(stderr, "Registers: ");
    for (int i = 0; i < 16; i++) 
        fprintf(stderr, "%d, ", V[i]);
    
    fprintf(stderr, "\nKeys:      ");
    for (int i = 0; i < 16; i++) 
        fprintf(stderr, "%d, ", self->keys[i]);

    fprintf(stderr, "\nVI = %02x, DT = %d, ST = %d\n", self->VI, self->DT, self->ST);
    fprintf(stderr, "--------------------------------------------------------------\n");
    */

    switch (instruction) {
        
        case 0x0000: { // Flow or display control            
            
            switch (byte) {
                
                case 0xE0: { // CLS - Clear screen

                    for (int i = 0; i < 2048; i++)
                        self->display[i] = 0;
                    
                    break;   
                }

                case 0xEE: { // # RET - Return from subroutine
                    self->stack_pointer -= 1;
                    self->PC = self->call_stack[self->stack_pointer];
                    return;
                }
            }

            break;
        }

        case 0x1000: { // JP addr - Inconditional jump
            self->PC = address;
            return;
        }

        case 0x2000: { // CALL addr - Call subroutine

            self->call_stack[self->stack_pointer] = self->PC + 2;
            self->stack_pointer += 1;
            self->PC = address;
            return;
        }

        case 0x3000: { // SE Vx, byte - Skip next instruction if Vx == byte

            if (V[x] == byte) {
                self->PC += 4;
                return;
            }

            break;
        }

        case 0x4000: { // SNE Vx, byte - Skip next instruction if Vx != byte

            //fprintf(stderr, "\nV[%d] = %d vs byte = %d", x, V[x], byte);

            if (V[x] != byte) {
                //fprintf(stderr, "\n0x4000 SKIP");
                self->PC += 4;
                return;
            }

            break;
        }

        case 0x5000: { // SE Vx, Vy - Skip next instruction if Vx == Vy

            if (V[x] == V[y]) {
                self->PC += 4;
                return;
            }

            break;
        }

        case 0x6000: { // LD Vx, byte - Write byte into Vx

            V[x] = byte;
            break;
        }

        case 0x7000: { // ADD Vx, byte - Adds byte to Vx

            V[x] += byte; // Overflow is handled naturally
            break;
        }

        case 0x8000: { // Several operations with registers

            switch (nibble) {

                case 0x0000: { // LD Vx, Vy - Store the value of Vy into Vx

                    V[x] = V[y];
                    break;
                }

                case 0x0001: { // OR Vx, Vy - Bitwise Vx OR Vy stored into Vx

                    V[x] = V[x] | V[y];
                    V[0xF] = 0;
                    break;
                }

                case 0x0002: { // AND Vx, Vy - Bitwise Vx AND Vy stored into Vx

                    V[x] = V[x] & V[y];
                    V[0xF] = 0;
                    break;
                }

                case 0x0003: { // XOR Vx, Vy - Bitwise Vx XOR Vy stored into Vx

                    V[x] = V[x] ^ V[y];
                    V[0xF] = 0;
                    break;
                }

                case 0x0004: { // ADD Vx, Vy - Add Vy into Vx

                    uint16_t carry = V[x] + V[y];

                    V[x] += V[y];
                    V[0xF] = (carry >> 8) & 1;
                    break;
                }

                case 0x0005: { // SUB Vx, Vy - Sub Vy from Vx

                    int16_t underflow = V[x] - V[y];

                    V[x] -= V[y];
                    
                    if (underflow < 0)
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    break;
                }

                //# SHR Vx {, Vy} - Shift Vx right by 1 bit, then store Vx LSB into VF
                case 0x0006: {

                    uint8_t old_vx = V[x];

                    V[x] >>= 1;
                    V[0xF] = old_vx & 1;
                    break;
                }

                case 0x0007: { // SUBN Vx {, Vy} - Sub Vx from Vy then store into Vx

                    int16_t underflow = V[y] - V[x];

                    V[x] = V[y] - V[x];

                    if (underflow < 0) 
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    break;
                }

                // SHL Vx {, Vy} - Shift Vx left by 1 bit, then store Vx MSB into VF
                case 0x000E: {

                    uint8_t old_vx = V[x];

                    V[x] <<= 1;

                    if ((old_vx & 0b10000000) > 0)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    break;
                }
            }

            break;
        }

        case 0x9000: { // SNE Vx, Vy - Skip next instruction if Vx != Vy

            if (V[x] != V[y]) {
                self->PC += 4;
                return;
            }

            break;
        }

        case 0xA000: { // LD I addr - Set register I to the address addr

            self->VI = address;
            break;
        }

        case 0xB000: { // JP V0, addr - Unconditional jump to addr + V0

            self->PC = address + V[0];
            return;
        }

        case 0xC000: { // RND Vx, byte - Generate random number to store in Vx

            int random_num = rand();

            if (random_num > 255)
                random_num &= 255;

            V[x] = random_num & byte;
            break;
        }

        case 0xD000: { // DRW Vx, Vy, nibble - Draw sprite

            uint8_t sprite[15] = {0};
            uint8_t sprite_size = 0;

            for (int i = 0; i < nibble; i++) {
                sprite[i] = self->memory[self->VI + i];
                sprite_size += 1;
            }

            uint8_t coordY = V[y] % 32;
            V[0xF] = 0;

            for (int i = 0; i < sprite_size; i++) {
                uint8_t line = sprite[i];

                if (coordY > 31)
                    break;

                uint8_t coordX = V[x] % 64;
                uint8_t number[8];

                //fprintf(stderr, "line = %03d, ", line);
                //fprintf(stderr, "number = ");

                for (int bit = 7; bit >= 0; bit--) {
                    if ((line & (int)pow(2, bit)) > 0)
                        number[bit] = 1;
                    else
                        number[bit] = 0;
                    //fprintf(stderr, "%d", number[bit]);
                }

                //fprintf(stderr, "\n");            

                for (int j = 7; j >= 0; j--) {
                    int digit = number[j];

                    if (coordX > 63)
                        continue;

                    int index = coordX + (coordY * 64);

                    if ((self->display[index] == 0) && (digit == 1)) {
                        self->display[index] = 1;
                    }
                    else {
                        if ((self->display[index] == 1) && (digit == 1)) {
                            self->display[index] = 0;
                            V[0xF] = 1;
                        }
                    }

                    //fprintf(stderr, "(%d, %d) = %d ", coordX, coordY, self->display[index]);
                    coordX += 1;
                }

                //fprintf(stderr, "\n");
                coordY += 1;
            }
            
            break;
        }

        case 0xE000: { // Skip to next instruction based on key pressed

            switch (byte) {

                // SKP Vx - Skip to next instruction if key[Vx] is pressed
                case 0x9E: {

                    if (self->keys[V[x]] == 1) {
                        self->PC += 4;
                        return;
                    }

                    break;
                }

                // SKP Vx - Skip to next instruction if key[Vx] is not pressed
                case 0xA1: {

                    if (self->keys[V[x]] == 0) { 
                        self->PC += 4;
                        return;
                    }

                    break;
                }
            }

            break;
        }

        case 0xF000: { // Miscellaneous opcodes

            switch (byte) {

                case 0x0007: { // LD Vx DT - Copy the delay timer value into Vx

                    V[x] = self->DT;
                    break;
                }

                case 0x000A: { // LD Vx Keydown - Wait for key press and release then store key value into Vx

                    // Return out of loop to update emulator

                    for (int i = 0; i < 16; i++) {

                        if ((self->keys[i] == 1) && (self->key_pressed == false)) {
                            self->key_pressed = true;
                            return;
                        }
                        else {
                            if ((self->keys[i] == 0) && (self->key_pressed == true)) {
                                V[x] = i;
                                self->PC += 2;
                                self->key_pressed = false;
                                return;
                            }
                        }
                    }

                    return;
                }

                case 0x0015: { // LD DT Vx - Set delay time to Vx value

                    self->DT = V[x];
                    break;
                }

                case 0x0018: { // LD ST Vx - Set sound timer to Vx value

                    self->ST = V[x];
                    break;
                }

                case 0x001E: { // ADD I Vx - Add Vx into I

                    self->VI += V[x]; // Overflow handled automatically
                    break;
                }

                case 0x0029: { // LD F Vx - Set I into the sprite location for hexadecimal digit Vx

                    self->VI = HEX_SPRITE_ADDRESS[V[x] % 16];
                    break;
                }

                case 0x0033: { // LD B VX - Store Vx value as BCD into I, I+1 and I+2

                    self->memory[self->VI] = V[x] / 100;
                    self->memory[self->VI + 1] = (V[x] % 100) / 10;
                    self->memory[self->VI + 2] = V[x] % 10;
                    break;
                }

                case 0x0055: { // LD [I] Vx - Copy V regisers into self.memory starting from I

                    for (int i = 0; i < (x + 1); i++)
                        self->memory[self->VI + i] = V[i];
                    
                    break;
                }

                // LD Vx [I] - Write V registers from self.memory starting from I
                case 0x0065:

                    for (int i = 0; i < (x + 1); i++)
                        V[i] = self->memory[self->VI + i];

                    break;
            }

            break;
        }

        default:
            perror("Invalid instruction!");
            exit(EXIT_FAILURE);
    }

    self->PC += 2;
    for (int i = 0; i < 16; i++)
        self->V[i] = V[i];
}
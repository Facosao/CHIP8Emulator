#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./include/CHIP8.h"
#include "./include/Font.h"

void init_memory(struct CHIP8* self) {
    for (int i = 0x100; i < 0x150; i++) {
        self->memory[i] = HEX_SPRITES[i % 0x100];
    }
}

void read_rom(struct CHIP8* self, char* file_path) {

    FILE* rom = fopen(file_path, "rb");

    if (rom == NULL) {
        perror("Failed to open ROM file!\n");
        SDL_Quit();
    }

    //printf("----- ROM Read -----\n");
    int index = 0x200;

    while (true) {
        char byte; 
        fread(&byte, 1, 1, rom);

        if (feof(rom) != 0) {
            break;
        }

        self->memory[index] = (uint8_t) byte;
        //printf("[0x%x] = %x\n", index, self->memory[index]);
        index += 1;

        if (index >= 4096)
            break;
    }

    //printf("----- ROM Finish -----\n");
    fclose(rom);
}

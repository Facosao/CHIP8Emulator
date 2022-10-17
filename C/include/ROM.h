#ifndef ROM_HEADER
#define ROM_HEADER

#include "CHIP8.h"

void init_memory(struct CHIP8* self);
void read_rom(struct CHIP8* self, char* file_path); 

#endif

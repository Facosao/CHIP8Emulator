import pygame
import sys
import Font


def init_memory(memory):

    for i in range(0xFFF + 1):
        memory.append(0)

    for i in range(16):
        for j in range(5):
            memory[Font.HEX_SPRITE_ADDRESS[i] + j] = Font.HEX_SPRITE[i][j]


def read_rom(memory):

    file_path: str = sys.argv[1]

    try:
        rom = open(file_path, "rb")
    except OSError:
        print("Failed to open ROM file!")
        pygame.quit()

    index = 0x200

    while True:

        byte = rom.read(1)

        if byte == b"":
            break

        byte = int.from_bytes(byte, "big")

        try:
            memory[index] = byte
        except IndexError:
            print("ROM exceeds memory size!")
            wait_for = input()
            pygame.quit()

        index += 1

    rom.close()
    return memory

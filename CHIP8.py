import random
import pygame
import Font


class CHIP8:
    def __init__(self) -> None:

        # Internal control
        self.key_pressed = False

        self.PC = 0x200
        self.DT = 0
        self.ST = 1
        self.timers = [0, 0]
        self.call_stack = []

        # Registers
        self.V = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.VI = 0

        self.keys = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.memory = []
        self.display = []

        for _ in range(2048):
            self.display.append(0)

    def run(self):

        instruction = self.memory[self.PC] >> 4  # HI 4 bits - 1st byte
        x = self.memory[self.PC] & 0xF  # LO 4 bits - 1st byte
        y = self.memory[self.PC + 1] >> 4  # HI 4 bits - 2nd byte
        nibble = self.memory[self.PC + 1] & 0xF  # LO 4 bits - 2nd byte

        # Only done for aesthetic reasons (e.g. 0x3000 instead of 3 or 0x3)
        instruction <<= 12

        byte = self.memory[self.PC + 1]
        address = (x << 8) | self.memory[self.PC + 1]

        V = self.V

        match instruction:

            case 0x0000:  # Flow or display control

                match byte:

                    case 0xE0:  # CLS - Clear screen

                        for i in range(2048):
                            self.display[i] = 0

                    case 0xEE:  # RET - Return from subroutine
                        self.PC = self.call_stack.pop()
                        return

            case 0x1000:  # JP addr - Inconditional jump
                self.PC = address
                return

            case 0x2000:  # CALL addr - Call subroutine

                self.call_stack.append(self.PC + 2)
                self.PC = address
                return

            case 0x3000:  # SE Vx, byte - Skip next instruction if Vx == byte

                if V[x] == byte:
                    self.PC += 4
                    return

            case 0x4000:  # SNE Vx, byte - Skip next instruction if Vx != byte

                if V[x] != byte:
                    self.PC += 4
                    return

            case 0x5000:  # SE Vx, Vy - Skip next instruction if Vx == Vy

                if V[x] == V[y]:
                    self.PC += 4
                    return

            case 0x6000:  # LD Vx, byte - Write byte into Vx

                V[x] = byte

            case 0x7000:  # ADD Vx, byte - Adds byte to Vx

                V[x] += byte

                if V[x] >= 256:
                    V[x] %= 256

            case 0x8000:  # Several operations with registers

                match nibble:

                    case 0x0000:  # LD Vx, Vy - Store the value of Vy into Vx

                        V[x] = V[y]

                    case 0x0001:  # OR Vx, Vy - Bitwise Vx OR Vy stored into Vx

                        V[x] = V[x] | V[y]
                        V[0xF] = 0

                    case 0x0002:  # AND Vx, Vy - Bitwise Vx AND Vy stored into Vx

                        V[x] = V[x] & V[y]
                        V[0xF] = 0

                    case 0x0003:  # XOR Vx, Vy - Bitwise Vx XOR Vy stored into Vx

                        V[x] = V[x] ^ V[y]
                        V[0xF] = 0

                    case 0x0004:  # ADD Vx, Vy - Add Vy into Vx

                        V[x] += V[y]

                        if V[x] >= 256:
                            V[x] %= 256
                            V[0xF] = 1
                        else:
                            V[0xF] = 0

                    case 0x0005:  # SUB Vx, Vy - Sub Vy from Vx

                        V[x] -= V[y]

                        if V[x] < 0:
                            V[x] %= 256
                            V[0xF] = 0
                        else:
                            V[0xF] = 1

                    # SHR Vx {, Vy} - Shift Vx right by 1 bit, then store Vx LSB into VF
                    case 0x0006:

                        old_vx = V[x]

                        V[x] >>= 1
                        V[0xF] = 1 if (old_vx & 1) == 1 else 0

                    case 0x0007:  # SUBN Vx {, Vy} - Sub Vx from Vy then store into Vx

                        V[x] = V[y] - V[x]

                        if V[x] < 0:
                            V[x] %= 256
                            V[0xF] = 0
                        else:
                            V[0xF] = 1

                    # SHL Vx {, Vy} - Shift Vx left by 1 bit, then store Vx MSB into VF
                    case 0x000E:

                        old_vx = V[x]

                        # 0xFF necessary due to Python bigInt
                        V[x] = (V[x] << 1) & 0x00FF

                        if old_vx & 0b10000000 > 0:
                            V[0xF] = 1
                        else:
                            V[0xF] = 0

            case 0x9000:  # SNE Vx, Vy - Skip next instruction if Vx != Vy

                if V[x] != V[y]:
                    self.PC += 4
                    return

            case 0xA000:  # LD I addr - Set register I to the address addr

                self.VI = address

            case 0xB000:  # JP V0, addr - Unconditional jump to addr + V0

                self.PC = address + V[0]
                return

            case 0xC000:  # RND Vx, byte - Generate random number to store in Vx

                V[x] = random.randint(0, 255) & byte

            case 0xD000:  # DRW Vx, Vy, nibble - Draw sprite

                sprite = []

                for j in range(0, nibble):
                    sprite.append(self.memory[self.VI + j])

                coordY = V[y] % 32
                V[0xF] = 0

                for line in sprite:

                    if coordY > 31:
                        break

                    coordX = V[x] % 64

                    number = []

                    for bit in range(7, (-1), (-1)):
                        if line & (2**bit) > 0:
                            number.append(1)
                        else:
                            number.append(0)

                    for digit in number:

                        if coordX > 63:
                            continue

                        index = coordX + (coordY * 64)

                        if (self.display[index] == 0) and (digit == 1):
                            self.display[index] = 1

                        elif (self.display[index] == 1) and (digit == 1):
                            self.display[index] = 0
                            V[0xF] = 1

                        coordX += 1

                    coordY += 1

            case 0xE000:  # Skip to next instruction based on key pressed

                match byte:

                    # SKP Vx - Skip to next instruction if key[Vx] is pressed
                    case 0x9E:

                        if self.keys[V[x]] == 1:
                            self.PC += 4
                            return

                    # SKP Vx - Skip to next instruction if key[Vx] is not pressed
                    case 0xA1:

                        if self.keys[V[x]] == 0:
                            self.PC += 4
                            return

            case 0xF000:  # Miscellaneous opcodes

                match byte:

                    case 0x0007:  # LD Vx DT - Copy the delay timer value into Vx

                        V[x] = self.timers[self.DT]

                    case 0x000A:  # LD Vx Keydown - Wait for key press and release then store key value into Vx

                        # Return out of loop to update emulator

                        for i in range(0, 16):

                            if self.keys[i] == 1 and self.key_pressed == False:
                                self.key_pressed = True
                                return

                            elif self.keys[i] == 0 and self.key_pressed == True:
                                V[x] = i
                                self.PC += 2
                                self.key_pressed = False
                                return

                        return

                    case 0x0015:  # LD DT Vx - Set delay time to Vx value

                        self.timers[self.DT] = V[x]

                    case 0x0018:  # LD ST Vx - Set sound timer to Vx value

                        self.timers[self.ST] = V[x]

                    case 0x001E:  # ADD I Vx - Add Vx into I

                        self.VI += V[x]

                        if self.VI > 0xFFF:
                            self.VI %= 0xFFF + 1

                    case 0x0029:  # LD F Vx - Set I into the sprite location for hexadecimal digit Vx

                        self.VI = Font.HEX_SPRITE_ADDRESS[V[x]]

                    case 0x0033:  # LD B VX - Store Vx value as BCD into I, I+1 and I+2

                        self.memory[self.VI] = V[x] // 100
                        self.memory[self.VI + 1] = (V[x] % 100) // 10
                        self.memory[self.VI + 2] = V[x] % 10

                    case 0x0055:  # LD [I] Vx - Copy V regisers into self.memory starting from I

                        for j in range(0, x + 1):
                            self.memory[self.VI + j] = V[j]

                    # LD Vx [I] - Write V registers from self.memory starting from I
                    case 0x0065:

                        for j in range(0, x + 1):
                            V[j] = self.memory[self.VI + j]

            case _:
                print("Invalid instruction!")
                pygame.quit()

        self.PC += 2
        self.V = V

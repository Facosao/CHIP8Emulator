use super::font;

const DT: usize = 0;
const ST: usize = 1;

// TODO: ADD SUPPORT FOR RANDOM - C000

pub struct CHIP8 {
    key_pressed: bool,
    pc: u16,
    pub timers: [u8; 2],
    call_stack: [u16; 16],
    sp: u16,
    v: [u8; 16],
    vi: u16,
    pub keys: [u8; 16],
    pub memory: [u8; 4096],
    pub display: [u8; 2048],
    pub queued_draw: bool,
    queued_x: u8,
    queued_y: u8,
    queued_nibble: u8,
}

impl CHIP8 {
    pub fn new() -> CHIP8 {
        CHIP8 {
            key_pressed: false,
            pc: 0x200,
            timers: [0; 2],
            call_stack: [0; 16],
            sp: 0,
            v: [0; 16],
            vi: 0,
            keys: [0; 16],
            memory: [0; 4096],
            display: [0; 2048],
            queued_draw: false,
            queued_x: 0,
            queued_y: 0,
            queued_nibble: 0,
        }
    }
}

#[allow(arithmetic_overflow)]
pub fn run(interpreter: &mut CHIP8, draw_call: bool) {

    fn _print_screen(interpreter: &CHIP8) {
        for i in (0..(2048 - 64 + 1)).step_by(64) {
            for j in 0..64 {
                print!("{}", interpreter.display[(i + j) as usize]);
            }
            print!("\n");
        }
        let mut _dummy: String = Default::default();
        std::io::stdin().read_line(&mut _dummy).unwrap();
    }

    let mut instruction = (interpreter.memory[interpreter.pc as usize] >> 4) as u16;  // HI 4 bits - 1st byte
    let x = (interpreter.memory[interpreter.pc as usize] & 0xF) as u8;  // -------------- LO 4 bits - 1st byte
    let y = (interpreter.memory[(interpreter.pc + 1) as usize] >> 4) as u8;  // --------- HI 4 bits - 2nd byte
    let nibble = interpreter.memory[(interpreter.pc + 1) as usize] & 0xF;  // ----------- LO 4 bits - 2nd byte

    // Only done for aesthetic reasons (e.g. 0x3000 instead of 3 or 0x3)
    instruction <<= 12;

    let byte = interpreter.memory[(interpreter.pc + 1) as usize];
    let address = (((x as u16) << 8) | (interpreter.memory[(interpreter.pc + 1) as usize] as u16)) as u16;

    let mut v = interpreter.v;
    // opcode = (self.memory[self.PC] << 8) | self.memory[self.PC + 1]
    let _opcode = ((interpreter.memory[interpreter.pc as usize] as u16) << 8) | (interpreter.memory[(interpreter.pc + 1) as usize] as u16);

    //dbg_hex!(interpreter.pc, instruction, x, y, nibble);
    //println!("PC: {:#x}, Opcode: {:#06x}, VI: {:#x}", interpreter.pc, _opcode, interpreter.vi);
    //println!("instruction: {:#x}, x: {:#x}, y: {:#x}, nibble: {:#x}", instruction, (x << 8), (y << 4), nibble);
    //println!("-------------------------------------");
    //let first = 0xa2;

    // Override: wait for v-blank
    //if draw_call { instruction = 0xD000; }
    //if draw_call { return; }

    match instruction {
        0x0000 => {  // Flow or display control
            match byte {
                0xE0 => {  // CLS - Clear screen
                    for i in 0..2048 {
                        interpreter.display[i] = 0;
                    }
                }

                0xEE => {  // RET - Return from subroutine 
                    interpreter.sp -= 1;
                    interpreter.pc = interpreter.call_stack[interpreter.sp as usize];
                    return;
                }

                _ => panic!("Invalid opcode in branch 0x0000!"),
            }
        }

        0x1000 => {  // JP addr - Incoditional jump
            //println!("address: {:#x}", address);
            interpreter.pc = address;
            return;
        }

        0x2000 => {  // CALL addr - Call subroutine
            interpreter.call_stack[interpreter.sp as usize] = interpreter.pc + 2;
            interpreter.sp += 1;
            interpreter.pc = address;
            return;
        }

        0x3000 => {  // SE Vx, byte - Skip next instruction if Vx == byte
            if v[x as usize] == byte {
                interpreter.pc += 4;
                return;
            }
        }

        0x4000 => {  // SNE Vx, byte - Skip next instruction if Vx != byte
            if v[x as usize] != byte {
                interpreter.pc += 4;
                return;
            }
        }

        0x5000 => {  // SE Vx, Vy - Skip next instruction if Vx == Vy
            if v[x as usize] == v[y as usize] {
                interpreter.pc += 4;
                return;
            }
        }

        0x6000 => {  // LD Vx, byte - Write byte into Vx
            v[x as usize] = byte;
        }

        0x7000 => {  // ADD Vx, byte - Adds byte to Vx
            v[x as usize] += byte;  // Can overflow
        }

        0x8000 => {  // Several operations with registers
            match nibble {
                0x0000 => {  // LD Vx, Vy - Store the value of Vy into Vx
                    v[x as usize] = v[y as usize];
                }

                0x0001 => {  // OR Vx, Vy - Bitwise Vx OR Vy stored into Vx
                    v[x as usize] |= v[y as usize];
                    v[0xF] = 0;
                }

                0x0002 => {  // AND Vx, Vy - Bitwise Vx AND Vy stored into Vx
                    v[x as usize] &= v[y as usize];
                    v[0xF] = 0;
                }

                0x0003 => {  // XOR Vx, Vy - Bitwise Vx XOR Vy stored into Vx
                    v[x as usize] ^= v[y as usize];
                    v[0xF] = 0;
                }

                0x0004 => {  // ADD Vx, Vy - Add Vy into Vx
                    let carry = (v[x as usize] as u16) + (v[y as usize] as u16);
                    v[x as usize] += v[y as usize];

                    if carry >= 256 {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                }

                0x0005 => {  // SUB Vx, Vy - Sub Vy from Vx
                    let carry = (v[x as usize] as i16) - (v[y as usize] as i16);
                    v[x as usize] -= v[y as usize];

                    if carry < 0 {
                        v[0xF] = 0;
                    } else {
                        v[0xF] = 1;
                    }
                }

                0x0006 => {  // SHR Vx {, Vy} - Shift Vx right by 1 bit, then store Vx LSB into VF
                    //let old_vx = v[x as usize];
                    //v[x as usize] >>= 1;
                    let old_v = v[y as usize];
                    v[x as usize] = v[y as usize] >> 1;

                    if (old_v & 1) == 1 {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                }

                0x0007 => {  // SUBN Vx {, Vy} - Sub Vx from Vy then store into Vx
                    let carry = (v[y as usize] as i16) - (v[x as usize] as i16);
                    v[x as usize] = v[y as usize] - v[x as usize];

                    if carry < 0 {
                        v[0xF] = 0;
                    } else {
                        v[0xF] = 1;
                    }
                }

                0x000E => {  // SHL Vx {, Vy} - Shift Vx left by 1 bit, then store Vx MSB into VF
                    //let old_vx = v[x as usize];
                    //v[x as usize] <<= 1;
                    let old_v = v[y as usize];
                    v[x as usize] = v[y as usize] << 1;


                    if (old_v & 0b10000000) > 0 {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                }

                _ => panic!("Invalid opcode in branch 0x8000!"),
            }

        }

        0x9000 => {  // SNE Vx, Vy - Skip next instruction if Vx != Vy
            if v[x as usize] != v[y as usize] {
                interpreter.pc += 4;
                return;
            }
        }

        0xA000 => {  // LD I addr - Set register I to the address addr
            interpreter.vi = address;
        }

        0xB000 => {  // JP V0, addr - Unconditional jump to addr + V0
            interpreter.pc = address + (v[0] as u16);
            return;
        }

        0xC000 => {  // RND Vx, byte - Generate random number to store in Vx
            v[x as usize] = 1 & byte; // TODO: Add RNG
        }

        0xD000 => {  // DRW Vx, Vy, nibble - Draw sprite
            interpreter.queued_draw = true;
            interpreter.queued_x = x;
            interpreter.queued_y = y;
            interpreter.queued_nibble = nibble;
            // Should also queue PC and VI
            // Store them in a vector, then iterate over them
            // once the draw instruction is called externally.

            // ALTERNATIVE METHOD:
            // Stall the CPU for the remaining cycles on the current frame
            if !draw_call {
                //interpreter.pc -= 2;
                return;
            }

            let mut sprite: [i16; 15] = [0; 15];

            for j in 0..nibble {
                sprite[j as usize] = (interpreter.memory[(interpreter.vi + (j as u16)) as usize]) as i16;
            }

            let mut coord_y = v[y as usize] % 32;
            v[0xF] = 0;

            for line in sprite.iter() {
                //let line = sprite[k];

                if coord_y > 31 {
                    break;
                }

                let mut coord_x = v[x as usize] % 64;
                let mut number: [u8; 8] = [0; 8];

                for bit in (0..=7).rev() {
                    if (line & (1 << bit)) > 0 {
                        number[bit as usize] = 1;
                    }
                }

                //println!("line = number new = {:?}", number);
                number.reverse();
                //println!("line = {:#03}, number = {:?}", line, number);

                for digit in number {
                    if coord_x > 63 {
                        continue;
                    }

                    let index = (coord_x as u16) + (coord_y as u16) * 64;
                    //print!("[{}]", index);

                    if (interpreter.display[index as usize] == 0) && (digit == 1) {
                        interpreter.display[index as usize] = 1;
                    } else if (interpreter.display[index as usize] == 1) && (digit == 1) {
                        interpreter.display[index as usize] = 0;
                        v[0xF] = 1;
                    }
                    
                    //print!("({}, {}) = {} ", coord_x, coord_y, digit);
                    //print!("({}, {}) = {}\n", coord_x, coord_y, interpreter.display[index as usize]);
                    coord_x += 1;                        
                }
                //print!("\n");
                coord_y += 1;
            }

            //interpreter.queued_draw = false;
            //return;

            //println!("-------------------------");
            //print_screen(&interpreter);
            //let mut _dummy: String = Default::default();
            //std::io::stdin().read_line(&mut _dummy).unwrap();
        }

        0xE000 => {  // Skip to next instruction based on key pressed
            match byte {
                0x9E => {  // SKP Vx - Skip to next instruction if key[Vx] is pressed
                    if interpreter.keys[v[x as usize] as usize] == 1 {
                        interpreter.pc += 4;
                        return;
                    }
                }

                0xA1 => {  // SKP Vx - Skip to next instruction if key[Vx] is not pressed
                    if interpreter.keys[v[x as usize] as usize] == 0 {
                        interpreter.pc += 4;
                        return;
                    }
                }

                _ => panic!("Invalid opcode in branch 0xE000!"),
            }
        }

        0xF000 => {  // Miscellaneous opcodes
            match byte {
                0x0007 => {  // LD Vx DT - Copy the delay timer value into Vx
                    v[x as usize] = interpreter.timers[DT];
                }

                0x000A => {  // LD Vx Keydown - Wait for key press and release then store key value into Vx
                    // Return out of loop to update emulator

                    for i in 0..16 {
                        if (interpreter.keys[i] == 1) && (interpreter.key_pressed == false) {
                            interpreter.key_pressed = true;
                            return;
                        } else if (interpreter.keys[i] == 0) && (interpreter.key_pressed == true) {
                            v[x as usize] = i as u8;
                            interpreter.pc += 2;
                            interpreter.key_pressed = false;
                            return;
                        }
                    }
                    return;
                }

                0x0015 => {  // LD DT Vx - Set delay time to Vx value
                    interpreter.timers[DT] = v[x as usize];
                }

                0x0018 => { // LD ST Vx - Set sound timer to Vx value
                    interpreter.timers[ST] = v[x as usize];
                }

                0x001E => {  // ADD I Vx - Add Vx into I
                    interpreter.vi += v[x as usize] as u16;
                }

                0x0029 => {  // LD F Vx - Set I into the sprite location for hexadecimal digit Vx
                    interpreter.vi = font::HEX_SPRITE_ADDRESS[(interpreter.v[x as usize] % 16) as usize];
                }

                0x0033 => {  // LD B VX - Store Vx value as BCD into I, I+1 and I+2
                    interpreter.memory[interpreter.vi as usize] = v[x as usize] / 100;
                    interpreter.memory[(interpreter.vi + 1) as usize] = (v[x as usize] % 100) / 10;
                    interpreter.memory[(interpreter.vi + 2) as usize] = v[x as usize] % 10;
                }

                0x0055 => {  // LD [I] Vx - Copy V regisers into self.memory starting from I
                    for j in 0..(x + 1) {
                        interpreter.memory[(interpreter.vi + (j as u16)) as usize] = v[j as usize];
                    }
                    interpreter.vi += (x as u16) + 1;
                }

                0x0065 => {  // LD Vx [I] - Write V registers from self.memory starting from I
                    for j in 0..(x + 1) {
                        v[j as usize] = interpreter.memory[(interpreter.vi + (j as u16)) as usize];
                    }

                    interpreter.vi += (x as u16) + 1;
                }

                _ => panic!("Invalid opcode in branch 0xF000!"),
            }
        }

        _ => panic!("Invalid instruction!"),
    }

    interpreter.pc += 2;
    interpreter.v = v;
}

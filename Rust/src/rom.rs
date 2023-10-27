use super::font;
use super::CHIP8;

pub fn init_memory(interpreter: &mut CHIP8) {
    for i in 0..16 {
        for j in 0..5 {
            interpreter.memory[(font::HEX_SPRITE_ADDRESS[i as usize] + j) as usize] = font::HEX_SPRITE[i as usize][j as usize];
        }
    }
}

pub fn read_rom(interpreter: &mut CHIP8) {
    let args = std::env::args().into_iter();
    //dbg!(&args);
    let file_path = args.last().unwrap();

    let rom = std::fs::read(file_path.clone()).unwrap();
    //println!("{}", file_path.clone());
    //println!("{}", rom.len());
    //std::process::exit(0);
    //let mut index: usize = 0x200;

    let limit = if (0x200 + rom.len()) < 4096 { 0x200 + rom.len() } else { 4096 };

    for i in 0x200..limit {
        //dbg!(rom[i - 0x200]);
        interpreter.memory[i] = rom[i - 0x200];
        //index += 1;
    }

    //std::process::exit(0);
}
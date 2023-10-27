pub mod chip8;
pub mod font;
pub mod controller;
pub mod rom;
pub mod screen;
pub mod timer;

use chip8::CHIP8;
use minifb::{Scale, ScaleMode, Window, WindowOptions};

const WIDTH: usize = 64;
const HEIGHT: usize = 32;

const FRAMERATE: u64 = 60;
const EMULATOR_FREQUENCY: u64 = 2000;

fn main() {
    if std::env::args().len() != 2 {
        println!("Usage: chip8-rs [rom-file-path.ch8]");
        std::process::exit(0);
    }
    
    let mut buffer: Vec<u32> = vec![0; WIDTH * HEIGHT];

    let mut window = Window::new(
        "CHIP-8 Emulator",
        WIDTH,
        HEIGHT,
        WindowOptions {
            resize: true,
            scale: Scale::X16,
            scale_mode: ScaleMode::AspectRatioStretch,
            ..WindowOptions::default()
        },
    )
    .unwrap_or_else(|e| {
        panic!("{}", e);
    });

    window.limit_update_rate(Some(std::time::Duration::from_micros(16667)));

    let mut interpreter = CHIP8::new();

    rom::init_memory(&mut interpreter);
    rom::read_rom(&mut interpreter);

    while window.is_open() {
        
        controller::set_keys(window.get_keys(), &mut interpreter);

        for _ in 0..(EMULATOR_FREQUENCY / FRAMERATE) {
            chip8::run(&mut interpreter, false);
        }

        if interpreter.queued_draw {
            chip8::run(&mut interpreter, true);
        }

        timer::timer_update(&mut interpreter);
        screen::draw(&mut window, &mut buffer, &interpreter);
    }
}

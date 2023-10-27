use minifb::Window;
use super::CHIP8;
use super::WIDTH;
use super::HEIGHT;

const fn from_u8_rgb(r: u8, g: u8, b: u8) -> u32 {
    let (r, g, b) = (r as u32, g as u32, b as u32);
    (r << 16) | (g << 8) | b
}

pub fn draw(window: &mut Window, buffer: &mut Vec<u32>, interpreter: &CHIP8) {
    for i in 0..buffer.len() {
        if interpreter.display[i] == 1 {
            buffer[i] = from_u8_rgb(33, 41, 70);
        } else {
            buffer[i] = from_u8_rgb(97, 134, 169);
        }
    }

    window.update_with_buffer(&buffer, WIDTH, HEIGHT).unwrap();
}

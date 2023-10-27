use super::CHIP8;
use::minifb::Key;

pub fn set_keys(keys: Vec<Key>, interpreter: &mut CHIP8) {  
    for key in interpreter.keys.iter_mut() {
        *key = 0;
    }
    
    keys.iter().for_each(|key| {
        match key {
            Key::X => interpreter.keys[0] = 1,
            Key::Key1 => interpreter.keys[1] = 1,
            Key::Key2 | Key::Up => interpreter.keys[2] = 1,
            Key::Key3 => interpreter.keys[3] = 1,
            Key::Q | Key::Left => interpreter.keys[4] = 1,
            Key::W => interpreter.keys[5] = 1,
            Key::E | Key::Right => interpreter.keys[6] = 1,
            Key::A => interpreter.keys[7] = 1,
            Key::S | Key::Down => interpreter.keys[8] = 1,
            Key::D => interpreter.keys[9] = 1,
            Key::Z => interpreter.keys[0xA] = 1,
            Key::C => interpreter.keys[0xB] = 1,
            Key::Key4 => interpreter.keys[0xC] = 1,
            Key::R => interpreter.keys[0xD] = 1,
            Key::F => interpreter.keys[0xE] = 1,
            Key::V => interpreter.keys[0xF] = 1,
            _ => (),
        }
    });
}

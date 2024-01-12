use std::str::FromStr;

use enigo::Key;
use serde::{Deserialize, Serialize};
use tauri_plugin_global_shortcut::{Code, Modifiers, Shortcut as TauriShortcut};

#[derive(Deserialize, Serialize, Clone, Debug, PartialEq)]
pub struct Shortcut {
    pub key: String,
    pub ctrl: bool,
    pub alt: bool,
    pub shift: bool,
}

impl Default for Shortcut {
    fn default() -> Self {
        Self {
            key: "Space".to_string(),
            ctrl: true,
            alt: false,
            shift: false,
        }
    }
}

impl Into<Modifiers> for Shortcut {
    fn into(self) -> Modifiers {
        let bits: u32 = (self.alt as u32 & Modifiers::ALT.bits())
            | (self.ctrl as u32) & Modifiers::CONTROL.bits()
            | self.shift as u32 & Modifiers::SHIFT.bits();
        Modifiers::from_bits_truncate(bits)
    }
}

impl Shortcut {
    pub fn is_ctrl_code(&self, key: &str) -> bool {
        self.ctrl && !self.alt && !self.shift && self.key == key
    }

    fn modifier_flags(&self) -> Modifiers {
        let bits: u32 = 0
            | self.ctrl.then(|| Modifiers::CONTROL.bits()).unwrap_or(0)
            | self.alt.then(|| Modifiers::ALT.bits()).unwrap_or(0)
            | self.shift.then(|| Modifiers::SHIFT.bits()).unwrap_or(0);
        Modifiers::from_bits_truncate(bits)
    }

    fn key_code(&self) -> Option<Code> {
        Code::from_str(&self.key).ok()
    }

    pub fn as_tauri_shortcut(&self) -> Option<TauriShortcut> {
        //
        // A few sanity checks here to prevent madness
        //

        if self.is_ctrl_code("KeyC") || self.is_ctrl_code("KeyV") {
            return None;
        }

        // Try and return a shortcut object
        let modifiers: Modifiers = self.modifier_flags();
        if let Some(code) = self.key_code() {
            Some(TauriShortcut::new(Some(modifiers), code))
        } else {
            None
        }
    }

    pub fn to_enigo_key(&self) -> Option<Key> {
        if let Some(code) = self.key_code() {
            match code {
                Code::Backquote => Some(Key::Layout('`')),
                Code::Backslash => Some(Key::Layout('\\')),
                Code::BracketLeft => Some(Key::Layout('[')),
                Code::BracketRight => Some(Key::Layout(']')),

                Code::Digit0 => Some(Key::Layout('0')),
                Code::Digit1 => Some(Key::Layout('1')),
                Code::Digit2 => Some(Key::Layout('2')),
                Code::Digit3 => Some(Key::Layout('3')),
                Code::Digit4 => Some(Key::Layout('4')),
                Code::Digit5 => Some(Key::Layout('5')),
                Code::Digit6 => Some(Key::Layout('6')),
                Code::Digit7 => Some(Key::Layout('7')),
                Code::Digit8 => Some(Key::Layout('8')),
                Code::Digit9 => Some(Key::Layout('9')),
                Code::Equal => Some(Key::Layout('=')),

                Code::IntlBackslash => Some(Key::Layout('|')),

                Code::KeyA => Some(Key::Layout('a')),
                Code::KeyB => Some(Key::Layout('d')),
                Code::KeyC => Some(Key::Layout('c')),
                Code::KeyD => Some(Key::Layout('d')),
                Code::KeyE => Some(Key::Layout('e')),
                Code::KeyF => Some(Key::Layout('f')),
                Code::KeyG => Some(Key::Layout('g')),
                Code::KeyH => Some(Key::Layout('h')),
                Code::KeyI => Some(Key::Layout('i')),
                Code::KeyJ => Some(Key::Layout('j')),
                Code::KeyK => Some(Key::Layout('k')),
                Code::KeyL => Some(Key::Layout('l')),
                Code::KeyM => Some(Key::Layout('m')),
                Code::KeyN => Some(Key::Layout('n')),
                Code::KeyO => Some(Key::Layout('o')),
                Code::KeyP => Some(Key::Layout('p')),
                Code::KeyQ => Some(Key::Layout('q')),
                Code::KeyR => Some(Key::Layout('r')),
                Code::KeyS => Some(Key::Layout('s')),
                Code::KeyT => Some(Key::Layout('t')),
                Code::KeyU => Some(Key::Layout('u')),
                Code::KeyV => Some(Key::Layout('v')),
                Code::KeyW => Some(Key::Layout('w')),
                Code::KeyX => Some(Key::Layout('x')),
                Code::KeyY => Some(Key::Layout('y')),
                Code::KeyZ => Some(Key::Layout('z')),

                Code::Comma => Some(Key::Layout(',')),
                Code::Minus => Some(Key::Layout('-')),
                Code::Period => Some(Key::Layout('.')),
                Code::Quote => Some(Key::Layout('\'')),
                Code::Semicolon => Some(Key::Layout(';')),
                Code::Slash => Some(Key::Layout('/')),
                Code::Backspace => Some(Key::Backspace),

                Code::Enter => Some(Key::Return),
                Code::Space => Some(Key::Space),
                Code::Tab => Some(Key::Tab),

                #[cfg(target_os = "windows")]
                Code::Convert => Some(Key::Convert),
                #[cfg(target_os = "windows")]
                Code::KanaMode => Some(Key::Kana),

                Code::Delete => Some(Key::Delete),
                Code::End => Some(Key::End),
                Code::Help => Some(Key::Help),
                Code::Home => Some(Key::Home),
                Code::Insert => Some(Key::Insert),
                Code::PageDown => Some(Key::PageDown),
                Code::PageUp => Some(Key::PageUp),

                Code::ArrowDown => Some(Key::DownArrow),
                Code::ArrowLeft => Some(Key::LeftArrow),
                Code::ArrowRight => Some(Key::RightArrow),
                Code::ArrowUp => Some(Key::UpArrow),

                #[cfg(target_os = "windows")]
                Code::Numpad0 => Some(Key::Numpad0),
                #[cfg(target_os = "windows")]
                Code::Numpad1 => Some(Key::Numpad1),
                #[cfg(target_os = "windows")]
                Code::Numpad2 => Some(Key::Numpad2),
                #[cfg(target_os = "windows")]
                Code::Numpad3 => Some(Key::Numpad3),
                #[cfg(target_os = "windows")]
                Code::Numpad4 => Some(Key::Numpad4),
                #[cfg(target_os = "windows")]
                Code::Numpad5 => Some(Key::Numpad5),
                #[cfg(target_os = "windows")]
                Code::Numpad6 => Some(Key::Numpad6),
                #[cfg(target_os = "windows")]
                Code::Numpad7 => Some(Key::Numpad7),
                #[cfg(target_os = "windows")]
                Code::Numpad8 => Some(Key::Numpad8),
                #[cfg(target_os = "windows")]
                Code::Numpad9 => Some(Key::Numpad9),
                #[cfg(target_os = "windows")]
                Code::NumpadAdd => Some(Key::Add),
                Code::NumpadBackspace => Some(Key::Backspace),
                Code::NumpadComma => Some(Key::Layout(',')),
                #[cfg(target_os = "windows")]
                Code::NumpadDivide => Some(Key::Divide),
                Code::NumpadEnter => Some(Key::Return),
                #[cfg(target_os = "windows")]
                Code::NumpadMultiply => Some(Key::Multiply),
                Code::NumpadParenLeft => Some(Key::Layout('(')),
                Code::NumpadParenRight => Some(Key::Layout(')')),
                #[cfg(target_os = "windows")]
                Code::NumpadStar => Some(Key::Multiply),
                #[cfg(target_os = "windows")]
                Code::NumpadSubtract => Some(Key::Subtract),
                Code::Escape => Some(Key::Escape),

                Code::CapsLock => Some(Key::CapsLock),
                Code::NumLock => Some(Key::Numlock),
                #[cfg(target_os = "windows")]
                Code::ScrollLock => Some(Key::Scroll),
                Code::Pause => Some(Key::Pause),

                #[cfg(target_os = "windows")]
                Code::BrowserBack => Some(Key::BrowserBack),
                #[cfg(target_os = "windows")]
                Code::BrowserFavorites => Some(Key::BrowserFavorites),
                #[cfg(target_os = "windows")]
                Code::BrowserForward => Some(Key::BrowserForward),
                #[cfg(target_os = "windows")]
                Code::BrowserHome => Some(Key::BrowserHome),
                #[cfg(target_os = "windows")]
                Code::BrowserRefresh => Some(Key::BrowserRefresh),
                #[cfg(target_os = "windows")]
                Code::BrowserSearch => Some(Key::BrowserSearch),
                #[cfg(target_os = "windows")]
                Code::BrowserStop => Some(Key::BrowserStop),

                #[cfg(target_os = "windows")]
                Code::LaunchApp1 => Some(Key::LaunchApp1),
                #[cfg(target_os = "windows")]
                Code::LaunchApp2 => Some(Key::LaunchApp2),
                #[cfg(target_os = "windows")]
                Code::LaunchMail => Some(Key::LaunchMail),
                Code::Select => Some(Key::Select),

                Code::F1 => Some(Key::F1),
                Code::F2 => Some(Key::F2),
                Code::F3 => Some(Key::F3),
                Code::F4 => Some(Key::F4),
                Code::F5 => Some(Key::F5),
                Code::F6 => Some(Key::F6),
                Code::F7 => Some(Key::F7),
                Code::F8 => Some(Key::F8),
                Code::F9 => Some(Key::F9),
                Code::F10 => Some(Key::F10),
                Code::F11 => Some(Key::F11),
                Code::F12 => Some(Key::F12),
                Code::F13 => Some(Key::F13),
                Code::F14 => Some(Key::F14),
                Code::F15 => Some(Key::F15),
                Code::F16 => Some(Key::F16),
                Code::F17 => Some(Key::F17),
                Code::F18 => Some(Key::F18),
                Code::F19 => Some(Key::F19),
                Code::F20 => Some(Key::F20),
                Code::F21 => Some(Key::F21),
                Code::F22 => Some(Key::F22),
                Code::F23 => Some(Key::F23),
                Code::F24 => Some(Key::F24),

                _ => None,
            }
        } else {
            None
        }
    }
}

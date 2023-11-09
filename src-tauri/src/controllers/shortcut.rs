use enigo::{Enigo, Key, KeyboardControllable};
use tauri::AppHandle;
use tauri_plugin_global_shortcut::{Error, GlobalShortcutExt};

use crate::models::shortcut::Shortcut;

use super::{Controller, SettingsController};

pub struct ShortcutController(pub AppHandle);
impl ShortcutController {
    ///
    /// Register the hotkey after verifying compatibility with both enigo and tauri
    pub fn register(&self, shortcut: &Shortcut) -> Result<(), Error> {
        if let (Some(shortcut), Some(_)) = (shortcut.as_tauri_shortcut(), shortcut.to_enigo_key()) {
            self.0.global_shortcut().register(shortcut)
        } else {
            Err(Error::GlobalHotkey("invalid shortcut".to_string()))
        }
    }

    ///
    /// Unregister the given shortcut
    pub fn unregister(&self, shortcut: &Shortcut) -> Result<(), Error> {
        if let Some(shortcut) = shortcut.as_tauri_shortcut() {
            self.0.global_shortcut().unregister(shortcut)
        } else {
            Err(Error::GlobalHotkey("invalid shortcut".to_string()))
        }
    }

    ///
    /// Send a key sequence, making sure to unregister the active shortcut
    fn send_sequence(&self, modifier: Key, key: char) {
        let configured_shortcut = SettingsController(self.0.clone())
            .read()
            .unwrap_or_default()
            .shortcut
            .to_enigo_key()
            .unwrap();
        let mut enigo = Enigo::new();

        // Reset modifiers
        enigo.key_up(Key::Control);
        enigo.key_up(Key::Alt);
        enigo.key_up(Key::Shift);
        enigo.key_up(configured_shortcut);

        enigo.key_down(modifier);
        enigo.key_click(Key::Layout(key));
        enigo.key_up(modifier);
    }

    ///
    /// Send a CTRL-C key sequence to the system
    pub fn send_copy(&self) {
        self.send_sequence(Key::Control, 'c');
    }

    ///
    /// Send a CTRL-V key sequence to the system
    pub fn send_paste(&self) {
        self.send_sequence(Key::Control, 'v');
    }
}

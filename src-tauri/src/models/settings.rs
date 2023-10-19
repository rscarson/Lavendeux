use serde::{Deserialize, Serialize};

use crate::ManagedValue;

use super::shortcut::Shortcut;

#[derive(Deserialize, Serialize, Clone, Debug)]
pub struct Settings {
    enhanced_clipboard: bool,
    silence_errors: bool,
    start_with_os: bool,

    start_script: String,
    shortcut: Shortcut,

    dark_theme: bool,
    language_code: String,
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            enhanced_clipboard: true,
            silence_errors: false,
            start_with_os: false,

            start_script: String::default(),
            shortcut: Shortcut::default(),

            dark_theme: false,
            language_code: "en".to_string(),
        }
    }
}

type ManagedSettings = ManagedValue<Settings>;

#[tauri::command]
pub fn read_settings(state: tauri::State<ManagedSettings>) -> Result<Settings, ()> {
    state.read().and_then(|v| Some(v.clone())).ok_or(())
}

#[tauri::command]
pub fn write_settings(settings: Settings, state: tauri::State<ManagedSettings>) -> Result<(), ()> {
    println!("Updating settings: {:?}", settings);
    state.write(settings)
}

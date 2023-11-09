use super::shortcut::Shortcut;
use serde::{Deserialize, Serialize};

#[derive(Deserialize, Serialize, Clone, Debug, PartialEq)]
pub struct Settings {
    pub enhanced_clipboard: bool,
    pub display_errors: bool,
    pub start_with_os: bool,

    pub start_script: String,
    pub shortcut: Shortcut,

    pub dark_theme: bool,
    pub language_code: String,
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            enhanced_clipboard: true,
            display_errors: true,
            start_with_os: false,

            start_script: String::default(),
            shortcut: Shortcut::default(),

            dark_theme: false,
            language_code: "en".to_string(),
        }
    }
}

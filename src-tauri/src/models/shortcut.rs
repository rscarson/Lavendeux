use serde::{Deserialize, Serialize};

#[derive(Deserialize, Serialize, Clone, Debug)]
pub struct Shortcut {
    key: String,
    ctrl: bool,
    alt: bool,
    shift: bool,
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

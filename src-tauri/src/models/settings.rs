use std::path::{Path, PathBuf};

use crate::bugcheck;

use super::shortcut::Shortcut;
use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Manager};

#[derive(Deserialize, Serialize, Clone, Debug, PartialEq)]
pub struct Settings {
    // exclude from serialization
    #[serde(skip)]
    root_dir: PathBuf,

    pub enhanced_clipboard: bool,
    pub display_errors: bool,
    pub start_with_os: bool,

    pub start_script: String,
    pub timeout_ms: u64,
    pub shortcut: Shortcut,

    pub dark_theme: bool,
    pub language_code: String,
}

// Default enhanced_clipboard to true on all but linux
#[cfg(target_os = "linux")]
const DEFAULT_ENHANCED_CLIPBOARD: bool = false;
#[cfg(not(target_os = "linux"))]
const DEFAULT_ENHANCED_CLIPBOARD: bool = true;

impl Default for Settings {
    fn default() -> Self {
        Self {
            root_dir: PathBuf::from("./"),

            enhanced_clipboard: DEFAULT_ENHANCED_CLIPBOARD,
            display_errors: true,
            start_with_os: false,

            start_script: String::default(),
            timeout_ms: 3000,
            shortcut: Shortcut::default(),

            dark_theme: false,
            language_code: "en".to_string(),
        }
    }
}

impl Settings {
    const CONFIG_FILENAME: &'static str = "lavendeux.config.json";
    const EXTENSIONS_DIR: &'static str = "extensions";

    pub fn default_root(&mut self, app: AppHandle) -> PathBuf {
        match app.path().app_config_dir() {
            Ok(root_dir) => root_dir,
            Err(e) => {
                bugcheck::general(
                    app,
                    "Error finding configuration directory; defaulting to ./",
                    &e.to_string(),
                );
                PathBuf::from("./")
            }
        }
    }

    pub fn set_root(&mut self, root_dir: PathBuf) {
        self.root_dir = root_dir;
    }

    ///
    /// Returns the directory into which everything is to be stored
    pub fn root_dir(&self) -> &Path {
        &self.root_dir
    }

    ///
    /// Returns the directory into which extensions are to be stored
    pub fn ext_dir(&self) -> PathBuf {
        self.root_dir.join(Self::EXTENSIONS_DIR)
    }

    ///
    /// Returns the filename into which configuration is to be stored
    pub fn filename(&self) -> PathBuf {
        self.root_dir.join(Self::CONFIG_FILENAME)
    }
}

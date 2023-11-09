use serde::{Deserialize, Serialize};
use std::path::PathBuf;
use tauri::{AppHandle, Manager};

#[derive(Default, Deserialize, Serialize, Clone, Debug)]
pub struct ConfigurationSettings {
    root_dir: PathBuf,
}

impl ConfigurationSettings {
    const CONFIG_FILENAME: &'static str = "lavendeux.config.json";
    const EXTENSIONS_DIR: &'static str = "extensions";

    pub fn new(app: AppHandle) -> Self {
        Self::with_dir(app.path().app_config_dir().unwrap_or(PathBuf::from("./")))
    }

    pub fn with_dir(dir: PathBuf) -> Self {
        Self { root_dir: dir }
    }

    ///
    /// Returns the directory into which everything are to be stored
    pub fn root_dir(&self) -> PathBuf {
        self.root_dir.clone()
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

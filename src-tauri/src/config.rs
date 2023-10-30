use serde::{Deserialize, Serialize};
use std::{
    fs,
    io::Write,
    path::{Path, PathBuf},
    process::Command,
};
use tauri::{AppHandle, Manager};

use crate::{
    bugcheck,
    models::{
        extension::{Blacklist, BlacklistManager, ManagedBlacklist},
        settings::{ManagedSettings, Settings, SettingsManager},
    },
};

#[derive(Serialize, Deserialize)]
struct WriteableSettings {
    config: Settings,
    disabled_extensions: Blacklist,
}

pub struct ConfigManager();
impl ConfigManager {
    const FILENAME: &'static str = "lavendeux.config.json";
    const EXTENSIONS: &'static str = "extensions";

    #[cfg(all(
        unix,
        not(any(target_os = "macos", target_os = "android", target_os = "emscripten"))
    ))]
    const OPEN_DIR_COMMAND: &str = "xdg-open";

    #[cfg(target_os = "windows")]
    const OPEN_DIR_COMMAND: &str = "explorer";

    #[cfg(target_os = "macos")]
    const OPEN_DIR_COMMAND: &str = "open";

    ///
    /// Attempt to create the directories needed for the app
    pub fn init(app: AppHandle) -> Result<(), std::io::Error> {
        fs::create_dir_all(Self::ext_dir(app))
    }

    ///
    /// Returns the directory in which configuration items are to be stored
    pub fn dir(app: AppHandle) -> PathBuf {
        app.path().app_config_dir().unwrap_or(PathBuf::from("./"))
    }

    pub fn ext_dir(app: AppHandle) -> PathBuf {
        Self::dir(app.clone()).join(Self::EXTENSIONS)
    }

    ///
    /// Returns the filename into which configuration is to be stored
    pub fn filename(app: AppHandle) -> PathBuf {
        Self::dir(app.clone()).join(Self::FILENAME)
    }

    ///
    /// Attempt to write settings to a file
    pub fn save(app: AppHandle) -> Result<(), std::io::Error> {
        let blacklist =
            BlacklistManager::read(app.state::<ManagedBlacklist>().inner()).unwrap_or_default();
        let settings =
            SettingsManager::read(app.state::<ManagedSettings>().inner()).unwrap_or_default();
        let s = WriteableSettings {
            config: settings,
            disabled_extensions: blacklist,
        };

        let s = serde_json::to_string_pretty(&s).expect("could not serialize settings");
        let mut file = fs::File::create(Self::filename(app.clone()))?;
        file.write_all(s.as_bytes())?;
        Ok(())
    }

    ///
    /// Attempt to load settings from a file
    pub fn load(app: AppHandle) -> (Settings, Blacklist) {
        if let Ok(contents) = fs::read_to_string(Self::filename(app.clone())) {
            if let Ok(config) = serde_json::from_str::<WriteableSettings>(&contents) {
                return (config.config, config.disabled_extensions);
            }
        }
        (Settings::default(), Blacklist::default())
    }

    ///
    /// Attempt to add a new extension
    pub fn add_extension(filename: &Path, app: AppHandle) -> Result<(), std::io::Error> {
        let dst_path = Self::ext_dir(app.clone()).join(filename.file_name().unwrap_or_default());
        println!(
            "Attempting to copy {} to {}",
            filename.display(),
            dst_path.display()
        );
        fs::copy(filename, dst_path)?;
        Ok(())
    }

    ///
    /// Attempt to delete an extension
    pub fn del_extension(filename: &Path, app: AppHandle) -> Result<(), std::io::Error> {
        let dst_path = Self::ext_dir(app.clone()).join(filename);
        fs::remove_file(dst_path)?;
        Ok(())
    }

    ///
    /// Open the config directory in the system file explorer
    pub fn open_config_dir(app: AppHandle) {
        let dir = Self::dir(app.clone());
        if Command::new(Self::OPEN_DIR_COMMAND)
            .arg(&dir)
            .spawn()
            .is_err()
        {
            bugcheck::general(
                app,
                "Could not display configuration",
                &format!(
                    "There was a problem opening the config directory:\n{}",
                    dir.display()
                ),
            )
        }
    }

    ///
    /// Open the config directory in the system file explorer
    pub fn open_ext_dir(app: AppHandle) {
        let dir = Self::ext_dir(app.clone());
        println!("{}", dir.display());
        if Command::new(Self::OPEN_DIR_COMMAND)
            .arg(&dir)
            .spawn()
            .is_err()
        {
            bugcheck::general(
                app,
                "Could not display configuration",
                &format!(
                    "There was a problem opening the config directory:\n{}",
                    dir.display()
                ),
            )
        }
    }
}

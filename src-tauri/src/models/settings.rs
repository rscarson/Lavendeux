use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Manager};
use tauri_plugin_autostart::ManagerExt;

use crate::{bugcheck, config, models::language::TranslatorManager, ManagedValue};

use super::{language::ManagedTranslator, shortcut::Shortcut, tray::update_tray};

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

pub type ManagedSettings = ManagedValue<Settings>;
pub struct SettingsManager {}
impl SettingsManager {
    pub fn read(state: &ManagedSettings) -> Option<Settings> {
        state.clone_inner()
    }
}

#[tauri::command]
pub fn open_config_dir(app: AppHandle) {
    config::ConfigManager::open_config_dir(app)
}

#[tauri::command]
pub fn read_settings(state: tauri::State<ManagedSettings>) -> Result<Settings, ()> {
    state.read().and_then(|v| Some(v.clone())).ok_or(())
}

#[tauri::command]
pub fn write_settings(
    settings: Settings,
    state: tauri::State<ManagedSettings>,
    app: AppHandle,
) -> Result<(), String> {
    let last_known_good = read_settings(state.clone()).or(Err("unknown error".to_string()))?;

    //
    // Perform housekeeping tasks
    //

    // Has anything changed?
    if settings == last_known_good {
        return Ok(());
    }

    // Hotkey registration
    if settings.shortcut != last_known_good.shortcut {
        settings
            .shortcut
            .register(&app)
            .or_else(|e| Err(e.to_string()))?;

        last_known_good.shortcut.unregister(&app).or_else(|_| {
            bugcheck::fatal(
                app.clone(),
                "error registering hotkey; manager is out of sync!",
            )
        })?;
    }

    // Set up autostart
    if settings.start_with_os != last_known_good.start_with_os {
        if settings.start_with_os {
            app.autolaunch().enable().or_else(|e| Err(e.to_string()))?;
        } else {
            app.autolaunch().disable().or_else(|e| Err(e.to_string()))?;
        }
    }

    // Set language
    if settings.language_code != last_known_good.language_code {
        if !TranslatorManager::set_language(
            settings.language_code.clone(),
            app.state::<ManagedTranslator>().inner(),
        ) {
            TranslatorManager::set_language(
                last_known_good.language_code.clone(),
                app.state::<ManagedTranslator>().inner(),
            );
        }
        app.emit(TranslatorManager::UPDATED_EVENT, ()).ok();
    }

    //
    // All is well - emit new settings
    //

    state
        .write(settings.clone())
        .or(Err("unknown error".to_string()))?;
    config::ConfigManager::save(app.clone()).ok();
    update_tray(&app).ok();
    app.emit("updated-settings", settings.clone()).ok();
    Ok(())
}

#[tauri::command]
pub fn app_exit(app: AppHandle) {
    app.exit(0);
}

use std::{
    collections::{HashMap, HashSet},
    ffi::OsStr,
    path::Path,
};

use lavendeux_parser::ParserState;
use tauri::{AppHandle, Manager};

use crate::{
    config::{self, ConfigManager},
    managed_value::ManagedValue,
};

use super::language::{ManagedTranslator, TranslatorManager};

pub type Extension = Result<lavendeux_parser::Extension, String>;
pub type Extensions = HashMap<String, Extension>;
pub type ManagedExtensions = ManagedValue<Extensions>;

pub struct ExtensionManager {}
impl ExtensionManager {
    const UPDATED_EVENT: &'static str = "updated-extensions";

    pub fn read(state: &ManagedExtensions) -> Option<Extensions> {
        state.clone_inner()
    }

    pub fn write(extensions: Extensions, state: &ManagedExtensions) -> Option<Extensions> {
        if state.write(extensions.clone()).is_ok() {
            Some(extensions)
        } else {
            None
        }
    }
}

pub type ManagedParserState = ManagedValue<ParserState>;
pub struct ParserStateManager {}
impl ParserStateManager {
    pub fn reload(
        app: AppHandle,
        state: &ManagedParserState,
    ) -> Result<Extensions, std::io::Error> {
        let lang_error = TranslatorManager::translate(
            "extensions\\lbl_disabled_by_user".to_string(),
            app.state::<ManagedTranslator>().inner(),
        )
        .unwrap_or("Disabled by User".to_string());

        let blacklist =
            BlacklistManager::read(app.state::<ManagedBlacklist>().inner()).unwrap_or_default();

        if let (Some(mut parser_state), Some(_path)) =
            (state.read(), ConfigManager::ext_dir(app.clone()).to_str())
        {
            let mut extensions = Extensions::default();
            for module in lavendeux_parser::rustyscript::Module::load_dir(_path)? {
                let extension = parser_state.extensions.load(module.filename());
                let filename = Path::new(module.filename())
                    .file_name()
                    .unwrap_or(OsStr::new(""))
                    .to_string_lossy()
                    .to_string();

                // Do not attempt to load disabled extensions
                if blacklist.contains(&filename) {
                    extensions.insert(filename, Err(lang_error.clone()));
                    continue;
                } else {
                    // Not disabled - attempt to load the extension
                    extensions.insert(filename, extension.clone().or_else(|e| Err(e.to_string())));
                    if let Ok(ext) = extension.clone() {
                        parser_state.extensions.add(module.filename(), ext)
                    }
                }
            }

            ExtensionManager::write(extensions.clone(), app.state::<ManagedExtensions>().inner());
            return Ok(extensions);
        };
        Ok(HashMap::default())
    }

    ///
    /// Add an item to the extensions tab
    pub fn remove(
        filename: &Path,
        app: AppHandle,
        state: &ManagedParserState,
    ) -> Option<Extensions> {
        ConfigManager::del_extension(filename, app.clone()).ok();
        Self::reload(app, state).ok()
    }

    ///
    /// Add an item to the extensions tab
    pub fn add(filename: &Path, app: AppHandle, state: &ManagedParserState) -> Option<Extensions> {
        ConfigManager::add_extension(filename, app.clone()).ok();
        Self::reload(app, state).ok()
    }
}

pub type Blacklist = HashSet<String>;
pub type ManagedBlacklist = ManagedValue<Blacklist>;

pub struct BlacklistManager {}
impl BlacklistManager {
    const UPDATED_EVENT: &'static str = "updated-blacklist";

    pub fn read(state: &ManagedBlacklist) -> Option<Blacklist> {
        state.clone_inner()
    }

    pub fn disable(filename: String, state: &ManagedBlacklist) -> Option<Blacklist> {
        if let Some(mut blacklist) = state.read() {
            blacklist.insert(filename);
            Some(blacklist.clone())
        } else {
            None
        }
    }

    pub fn enable(filename: String, state: &ManagedBlacklist) -> Option<Blacklist> {
        if let Some(mut blacklist) = state.read() {
            blacklist.remove(&filename);
            Some(blacklist.clone())
        } else {
            None
        }
    }
}

#[tauri::command]
pub fn open_ext_dir(app: AppHandle) {
    config::ConfigManager::open_ext_dir(app)
}

#[tauri::command]
pub fn read_blacklist(state: tauri::State<ManagedBlacklist>) -> Result<Blacklist, ()> {
    BlacklistManager::read(state.inner()).ok_or(())
}

#[tauri::command]
pub fn disable_extension(
    filename: String,
    state: tauri::State<ManagedBlacklist>,
    window: tauri::Window,
    app: AppHandle,
) -> Result<Blacklist, ()> {
    if let Some(blacklist) = BlacklistManager::disable(filename, state.inner()) {
        window
            .emit(BlacklistManager::UPDATED_EVENT, blacklist.clone())
            .ok();
        reload_extensions(app.state::<ManagedParserState>(), window, app.clone()).ok();
        config::ConfigManager::save(app.clone()).ok();
        Ok(blacklist)
    } else {
        Err(())
    }
}

#[tauri::command]
pub fn enable_extension(
    filename: String,
    state: tauri::State<ManagedBlacklist>,
    window: tauri::Window,
    app: AppHandle,
) -> Result<Blacklist, ()> {
    if let Some(blacklist) = BlacklistManager::enable(filename, state.inner()) {
        window
            .emit(BlacklistManager::UPDATED_EVENT, blacklist.clone())
            .ok();
        reload_extensions(app.state::<ManagedParserState>(), window, app.clone()).ok();
        config::ConfigManager::save(app.clone()).ok();
        Ok(blacklist)
    } else {
        Err(())
    }
}

#[tauri::command]
pub fn reload_extensions(
    state: tauri::State<ManagedParserState>,
    window: tauri::Window,
    app: AppHandle,
) -> Result<Extensions, ()> {
    match ParserStateManager::reload(app, state.inner()) {
        Ok(e) => {
            window.emit(ExtensionManager::UPDATED_EVENT, e.clone()).ok();
            Ok(e)
        }
        Err(_) => Err(()),
    }
}

#[tauri::command]
pub fn read_extensions(state: tauri::State<ManagedExtensions>) -> Result<Extensions, ()> {
    match ExtensionManager::read(state.inner()) {
        Some(e) => Ok(e),
        None => Err(()),
    }
}

#[tauri::command]
pub fn add_extension(
    filename: String,
    state: tauri::State<ManagedParserState>,
    window: tauri::Window,
    app: AppHandle,
) -> Result<Extensions, ()> {
    match ParserStateManager::add(Path::new(&filename), app, state.inner()) {
        Some(e) => {
            window.emit(ExtensionManager::UPDATED_EVENT, e.clone()).ok();
            Ok(e)
        }
        None => Err(()),
    }
}

#[tauri::command]
pub fn del_extension(
    filename: String,
    state: tauri::State<ManagedParserState>,
    window: tauri::Window,
    app: AppHandle,
) -> Result<Extensions, ()> {
    match ParserStateManager::remove(Path::new(&filename), app, state.inner()) {
        Some(e) => {
            window.emit(ExtensionManager::UPDATED_EVENT, e.clone()).ok();
            Ok(e)
        }
        None => Err(()),
    }
}

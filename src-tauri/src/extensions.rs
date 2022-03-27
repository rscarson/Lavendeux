use tauri::{AppHandle, Manager};
use std::path::{PathBuf, Path};
use super::settings;
use crate::state::{State, SharedState};
use std::process::Command;

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
const OPEN_DIR_COMMAND : &str = "xdg-open";

#[cfg(target_os="windows")]
const OPEN_DIR_COMMAND : &str = "explorer";

#[cfg(target_os="macos")]
const OPEN_DIR_COMMAND : &str = "open";

/// Reload extensions
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
pub fn reload_extensions(app_handle: AppHandle, state: &mut State) -> Result<(), String> {
	if let Err(e) = state.parser.extensions.load_all(&state.settings.extension_dir) {
		Err(e.to_string())
	} else {
		app_handle.emit_all("extensions", state.parser.extensions.all()).unwrap();
		Ok(())
	}
}

/// Open the extension directory in the system explorer
/// 
/// # Arguments
/// * `state` - Current application state
#[tauri::command]
pub fn open_extensions_dir(state: tauri::State<SharedState>) -> Option<settings::Settings> {
	let lock = state.0.lock().ok()?;
    Command::new(OPEN_DIR_COMMAND).arg(&lock.settings.extension_dir).spawn().ok()?;
	None
}

/// Reload extensions
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn reload_all_extensions(app_handle: AppHandle, state: tauri::State<SharedState>) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			match reload_extensions(app_handle, &mut lock) {
				Ok(_) => Ok(()),
				Err(e) => Err(e)
			}
		},
		None => Err("Could not access settings".to_string())
	}
}

/// Disable an extension
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
/// * `src_path` - Path to the extension
#[tauri::command]
pub fn disable_extension(app_handle: AppHandle, state: tauri::State<SharedState>, src_path: &str) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			// Get a path to the disabled extensions
			let mut path = PathBuf::new();
			path.push(&lock.settings.extension_dir);
			path.push("disabled_extensions");

			// Build destination path
			let dest: PathBuf = [
				path.to_str().ok_or("unicode error")?.to_string(),
				Path::new(src_path).file_name().ok_or("invalid filename")?.to_str().ok_or("unicode error")?.to_string()
			].iter().collect();

			// Move file
			match std::fs::rename(src_path, dest.as_path().to_str().ok_or("unicode error")?) {
				Ok(_) => {
					reload_extensions(app_handle, &mut lock)?;
					Ok(())
				},
				Err(e) => Err(e.to_string())
			}
		},
		None => Err("Could not access settings".to_string())
	}
}

/// Load an extension
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
/// * `src_path` - Path to the extension
#[tauri::command]
pub fn import_extension(app_handle: AppHandle, state: tauri::State<SharedState>, src_path: &str) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			// Build destination path
			let dest: PathBuf = [
				lock.settings.extension_dir.clone(), 
				Path::new(src_path).file_name().ok_or("invalid filename")?.to_str().ok_or("unicode error")?.to_string()
			].iter().collect();

			// Copy the file to the extensions directory
			match std::fs::copy(src_path, dest.as_path().to_str().ok_or("unicode error")?) {
				Ok(_) => {
					reload_extensions(app_handle, &mut lock)?;
					Ok(())
				},
				Err(e) => Err(e.to_string())
			}
		},
		None => Err("Could not access settings".to_string())
	}
}
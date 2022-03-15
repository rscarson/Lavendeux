use tauri::{AppHandle, Manager};
use std::path::{PathBuf, Path};
use super::{settings, SharedState};
use lavendeux_parser::Extension;
use crate::State;
use std::process::Command;

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
const OPEN_DIR_COMMAND : &str = "xdg-open";

#[cfg(target_os="windows")]
const OPEN_DIR_COMMAND : &str = "explorer";

#[cfg(target_os="macos")]
const OPEN_DIR_COMMAND : &str = "open";

pub fn open_dir(path: &str) {
    Command::new(OPEN_DIR_COMMAND).arg(path).spawn().unwrap();
}

pub fn reload_extension(app_handle: AppHandle, state: &mut State) -> Result<(), String> {
	match Extension::load_all(&state.settings.extension_dir) {
		Ok(ex) => { state.parser.extensions = ex; },
		Err(e) => { return Err(e.to_string()); }
	}
	app_handle.emit_all("extensions", state.parser.extensions.clone()).unwrap();
	Ok(())
}

#[tauri::command]
pub fn open_extensions_dir(state: tauri::State<SharedState>) -> Option<settings::Settings> {
	let lock = state.0.lock().ok()?;
	open_dir(&lock.settings.extension_dir);
	None
}

#[tauri::command]
pub fn reload_extensions(app_handle: AppHandle, state: tauri::State<SharedState>) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			match reload_extension(app_handle, &mut lock) {
				Ok(_) => Ok(()),
				Err(e) => Err(e.to_string())
			}
		},
		None => Err("Could not access settings".to_string())
	}
}

#[tauri::command]
pub fn disable_extension(app_handle: AppHandle, state: tauri::State<SharedState>, src_path: &str) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			let mut path = PathBuf::new();
			path.push(&lock.settings.extension_dir);
			path.push("disabled_extensions");

			let dest: PathBuf = [
				path.to_str().ok_or("unicode error")?.to_string(),
				Path::new(src_path).file_name().ok_or("invalid filename")?.to_str().ok_or("unicode error")?.to_string()
			].iter().collect();
			match std::fs::rename(src_path, dest.as_path().to_str().ok_or("unicode error")?) {
				Ok(_) => {
					reload_extension(app_handle, &mut lock)?;
					Ok(())
				},
				Err(e) => Err(e.to_string())
			}
		},
		None => Err("Could not access settings".to_string())
	}
}

#[tauri::command]
pub fn import_extension(app_handle: AppHandle, state: tauri::State<SharedState>, src_path: &str) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			let dest: PathBuf = [
				lock.settings.extension_dir.clone(), 
				Path::new(src_path).file_name().ok_or("invalid filename")?.to_str().ok_or("unicode error")?.to_string()
			].iter().collect();
			match std::fs::copy(src_path, dest.as_path().to_str().ok_or("unicode error")?) {
				Ok(_) => {
					reload_extension(app_handle, &mut lock)?;
					Ok(())
				},
				Err(e) => Err(e.to_string())
			}
		},
		None => Err("Could not access settings".to_string())
	}
}
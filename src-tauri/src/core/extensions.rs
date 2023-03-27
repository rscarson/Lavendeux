use lavendeux_parser::Extension;

use crate::core::State;
use crate::utils::fs;

/// Reload extensions
/// 
/// # Arguments
/// * `state` - Current application state
pub fn reload(state: &mut State) -> Result<Vec<Extension>, String> {
	match state.parser.extensions.load_all(&state.settings.inner_settings.extension_dir) {
		Ok(_) => Ok(state.parser.extensions.all()),
		Err(e) => {
			let error = format!("Error reloading extensions: {}", e);
			state.logger.error(&error);
			Err(error)
		}
	}
}

/// Open the extension directory in the system explorer
/// 
/// # Arguments
/// * `state` - Current application state
pub fn open_extensions_dir(state: &mut State) {
    if let Err(e) = fs::open(state.settings.inner_settings.extension_dir.clone()) {
		state.logger.error(&format!("Error opening extensions directory: {}", e));
	}
}

/// Get all extensions
/// 
/// # Arguments
/// * `state` - Current application state
pub fn all(state: &mut State) -> Result<Vec<Extension>, String> {
	Ok(state.parser.extensions.all())
}

/// Disable an extension
/// 
/// # Arguments
/// * `state` - Current application state
/// * `src_path` - Path to the extension
pub fn disable(state: &mut State, src_path: &str) -> Result<Vec<Extension>, String> {
	state.logger.debug(&format!("Disabling extension at '{}'", src_path));

	// Get a path for the disabled extension
	let dst_path = fs::compile_path(&[
		state.settings.inner_settings.extension_dir.clone(),
		"disabled_extensions".to_string(),
		fs::filename(src_path.to_string())?
	])?;

	// Move file
	match std::fs::rename(src_path, dst_path.clone()) {
		Ok(_) => {
			state.parser.extensions.remove(src_path);
			Ok(state.parser.extensions.all())
		},
		Err(e) => {
			state.logger.error(&format!("Could not move file to {}: {}", dst_path, e));
			Err(e.to_string())
		}
	}
}

/// Load an extension
/// 
/// # Arguments
/// * `state` - Current application state
/// * `src_path` - Path to the extension
pub fn import(state: &mut State, src_path: &str) -> Result<Vec<Extension>, String> {
	state.logger.debug(&format!("Importing '{}' as an extension", src_path));

	let dst_path = fs::compile_path(&[
		state.settings.inner_settings.extension_dir.clone(),
		fs::filename(src_path.to_string())?
	])?;

	// Copy the file to the extensions directory
	match fs::copy(src_path.to_string(), dst_path.clone()) {
		Ok(_) => {
			Ok(reload(state)?)
		},
		Err(e) => {
			state.logger.error(&format!("Could not copy extension to {}: {}", dst_path, e));
			Err(e.to_string())
		}
	}
}
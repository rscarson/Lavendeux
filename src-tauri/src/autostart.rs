use std::env::current_exe;

////////////////////////////////////////
/// Windows implementation
////////////////////////////////////////

#[cfg(target_os="windows")]
extern crate winreg;
#[cfg(target_os="windows")]
use winreg::{enums::*, RegKey};

#[cfg(target_os="windows")]
pub fn set() -> Option<String> {
	let hkcu = RegKey::predef(HKEY_CURRENT_USER);
	match hkcu.create_subkey("Software\\Microsoft\\Windows\\CurrentVersion\\Run") {
		Ok((key, _)) => {
			match current_exe() {
				Ok(self_path) => {
					match key.set_value("Lavendeux", &self_path.to_str()?) {
						Ok(_) => None,
						Err(e) => Some(format!("Could not write value: {}", e.to_string()))
					}
				},
				Err(e) => Some(format!("Could not find myself?: {}", e.to_string()))
			}
		},
		Err(e) => Some(format!("Unable to open registry: {}", e.to_string()))
	}
}

#[cfg(target_os="windows")]
pub fn clear() -> Option<String> {
	let hkcu = RegKey::predef(HKEY_CURRENT_USER);
	match hkcu.create_subkey("Software\\Microsoft\\Windows\\CurrentVersion\\Run") {
		Ok((key, _)) => {
			match key.delete_value("Lavendeux") {
				_ => None
			}
		},
		Err(e) => Some(format!("Unable to open registry: {}", e.to_string()))
	}
}

////////////////////////////////////////
/// Linux implementation
////////////////////////////////////////

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
pub fn set() -> Option<String> {
    Some("Autostart not implemented for Linux")
}

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
pub fn clear() -> Option<String> {
    Some("Autostart not implemented for Linux")
}

////////////////////////////////////////
/// Mac OSX implementation
////////////////////////////////////////

#[cfg(target_os="macos")]
pub fn set() -> Option<String> {
    Some("Autostart not implemented for OSX")
}

#[cfg(target_os="macos")]
pub fn clear() -> Option<String> {
    Some("Autostart not implemented for OSX")
}
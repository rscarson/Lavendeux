use std::env::current_exe;

extern crate winreg;
use winreg::{enums::*, RegKey};

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
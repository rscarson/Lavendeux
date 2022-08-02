use std::fs;
use dirs::home_dir;
use std::env::current_exe;

pub fn set() -> Option<String> {
	let mut path = home_dir().unwrap_or_default();
	path.push(".config");
	path.push("autostart");
	fs::create_dir_all(path.to_str()?).ok()?;
	path.push("lavendeux.desktop");
	let autostart_path = path.to_str()?;

	match current_exe() {
		Ok(self_path) => {
			fs::write(autostart_path, format!("\
			[Desktop Entry]\n\
			Exec={}\n\
			Type=Application\
			", self_path.to_str()?)).ok()?;
			None
		},
		Err(e) => Some(format!("Could not find myself?: {}", e.to_string()))
	}
}

pub fn clear() -> Option<String> {
	let mut path = home_dir().unwrap_or_default();
	path.push(".config");
	path.push("autostart");
	path.push("lavendeux.desktop");
	let autostart_path = path.to_str()?;
	fs::remove_file(autostart_path).ok()?;
	None
}
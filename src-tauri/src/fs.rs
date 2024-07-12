use crate::bugcheck;
use std::{fs, path::Path, process::Command};
use tauri::AppHandle;

pub struct FsUtils();

impl FsUtils {
    #[cfg(all(
        unix,
        not(any(target_os = "macos", target_os = "android", target_os = "emscripten"))
    ))]
    const OPEN_DIR_COMMAND: &str = "xdg-open";

    #[cfg(target_os = "windows")]
    const OPEN_DIR_COMMAND: &'static str = "explorer";

    #[cfg(target_os = "macos")]
    const OPEN_DIR_COMMAND: &'static str = "open";

    ///
    /// Open a directory in the system browser
    pub fn open_dir(app: AppHandle, dir: &Path) {
        if Command::new(Self::OPEN_DIR_COMMAND)
            .arg(dir)
            .spawn()
            .is_err()
        {
            bugcheck::general(
                app,
                "Could not open directory",
                &format!(
                    "There was a problem opening a directory:\n{}",
                    dir.display()
                ),
            )
        }
    }

    ///
    /// Get the filename from a path as a string
    pub fn basename(filename: &str) -> Option<String> {
        if let Some(basename) = Path::new(filename).file_name() {
            if let Some(s) = basename.to_str() {
                return Some(s.to_string());
            }
        }
        None
    }

    ///
    /// Attempt to copy a file
    pub fn copy(filename: &Path, destination: &Path) -> Result<(), std::io::Error> {
        let dst_path = destination.join(filename.file_name().unwrap_or_default());
        fs::copy(filename, dst_path).and(Ok(()))
    }

    ///
    /// Attempt to delete a file
    pub fn delete(filename: &Path, destination: &Path) -> Result<(), std::io::Error> {
        let dst_path = destination.join(filename.file_name().unwrap_or_default());
        fs::remove_file(dst_path).and(Ok(()))
    }
}

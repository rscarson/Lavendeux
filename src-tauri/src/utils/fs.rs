use std::path::{PathBuf, Path};
use std::process::Command;

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
const OPEN_DIR_COMMAND : &str = "xdg-open";

#[cfg(target_os="windows")]
const OPEN_DIR_COMMAND : &str = "explorer";

#[cfg(target_os="macos")]
const OPEN_DIR_COMMAND : &str = "open";

/// Construct a path from pieces
/// 
/// # Arguments
/// * `location` - Path to the file or folder
pub fn compile_path(location: &[String]) -> Result<String, String> {
    let buf : PathBuf = location.iter().collect();
    match buf.as_path().to_str() {
        Some(s) => Ok(s.to_string()),
        None => Err("Unicode error while creating path".to_string())
    }
}

/// Construct a path from pieces
/// 
/// # Arguments
/// * `root` - Path to merge into
/// * `location` - Path to the file or folder
pub fn merge_paths(root: &PathBuf, location: &[String]) -> Result<String, String> {
    let mut path = root.clone();
    for piece in location {
        path.push(piece);
    }
    
    match path.as_path().to_str() {
        Some(s) => Ok(s.to_string()),
        None => Err("Unicode error while creating path".to_string())
    }
}

/// Open a file or folder in the system explorer, returning an error if one occured
/// 
/// # Arguments
/// * `location` - Path to the file or folder
pub fn open(location: String) -> Result<(), String> {
    if let Err(e) = Command::new(OPEN_DIR_COMMAND).arg(&location).spawn() {
		Err(format!("Error opening directory: {}", e))
	} else {
        Ok(())
    }
}

/// Get the last component in a path
/// 
/// # Arguments
/// * `location` - Path to the file
pub fn filename(location: String) -> Result<String, String> {
    Ok(Path::new(&location).file_name().ok_or("invalid filename")?.to_str().ok_or("unicode error")?.to_string())
}

/// Copy a file
/// 
/// # Arguments
/// * `src` - Path to the source file
/// * `dst` - Target path 
pub fn copy(src: String, dst: String) -> Result<(), String> {
    match std::fs::copy(src, dst) {
        Ok(_) => Ok(()),
        Err(e) => Err(e.to_string())
    }
}

#[cfg(test)]
mod test_util_fs {
	use super::*;

	#[test]
	fn test_compile_path() {
        let path = compile_path(&[ "test1".to_string(), "test2".to_string(), "test3".to_string() ]).unwrap();
        
        #[cfg(target_os="windows")]
        assert_eq!("test1\\test2\\test3", path);
        
        #[cfg(not(target_os="windows"))]
        assert_eq!("test1/test2/test3", path);
	}

	#[test]
    fn test_merge_paths() {
        let pathbuf: PathBuf = ["test1"].iter().collect();
        let path = merge_paths(&pathbuf, &[ "test2".to_string(), "test3".to_string() ]).unwrap();
        
        #[cfg(target_os="windows")]
        assert_eq!("test1\\test2\\test3", path);
        
        #[cfg(not(target_os="windows"))]
        assert_eq!("test1/test2/test3", path);
    }

	#[test]
    fn test_filename() {
        assert_eq!(filename("test/filename.ext".to_string()).unwrap(), "filename.ext");
        assert_eq!(filename("test\\filename.ext".to_string()).unwrap(), "filename.ext");
        assert_eq!(filename("filename.ext".to_string()).unwrap(), "filename.ext");
    }
}
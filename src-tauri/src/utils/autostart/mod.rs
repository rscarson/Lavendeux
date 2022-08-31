#[cfg(target_os="windows")]
mod windows;
#[cfg(target_os="windows")]
pub use windows::*;

#[cfg(target_os="linux")]
mod linux;
#[cfg(target_os="linux")]
pub use linux::*;

#[cfg(target_os="macos")]
mod macos;
#[cfg(target_os="macos")]
pub use macos::*;

/// Update autostart status
/// 
/// # Arguments
/// * `status` - true to enable autostart
pub fn update(status: bool) -> Option<String> {
    if status { set() } else { clear() }
}
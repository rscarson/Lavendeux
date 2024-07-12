use tauri::AppHandle;
use tauri_plugin_clipboard_manager::ClipboardExt;

pub struct ClipboardController(pub AppHandle);
impl ClipboardController {
    const MAX_CLIP_TRIES: usize = 5;

    pub fn read(&self) -> Option<String> {
        for _ in 0..Self::MAX_CLIP_TRIES {
            match self.0.clipboard().read_text() {
                Ok(text) => return Some(text),
                Err(e) => {
                    println!("{e}");
                }
            }
        }
        None
    }

    pub fn write(&self, text: &str) -> bool {
        for _ in 0..Self::MAX_CLIP_TRIES {
            match self.0.clipboard().write_text(text) {
                Ok(_) => return true,
                Err(e) => {
                    println!("{e}");
                }
            }
        }

        false
    }
}

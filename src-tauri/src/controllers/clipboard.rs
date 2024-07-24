use crate::error::Error;
use tauri::AppHandle;
use tauri_plugin_clipboard_manager::ClipboardExt;

pub struct ClipboardController(pub AppHandle);
impl ClipboardController {
    const MAX_CLIP_TRIES: usize = 5;

    pub fn read(&self) -> Result<String, Error> {
        let mut last_result = Ok(String::new());
        for _ in 0..Self::MAX_CLIP_TRIES {
            match self.0.clipboard().read_text() {
                Ok(text) => return Ok(text),
                Err(e) => last_result = Err(e),
            }
        }
        Ok(last_result?)
    }

    pub fn write(&self, text: &str) -> Result<(), Error> {
        let mut last_result = Ok(());
        for _ in 0..Self::MAX_CLIP_TRIES {
            match self.0.clipboard().write_text(text) {
                Ok(_) => return Ok(()),
                Err(e) => last_result = Err(e),
            }
        }
        Ok(last_result?)
    }
}

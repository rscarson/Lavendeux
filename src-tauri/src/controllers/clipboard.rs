use tauri::AppHandle;
use tauri_plugin_clipboard_manager::{ClipKind, ClipboardContents, ClipboardExt};

pub struct ClipboardController(pub AppHandle);
impl ClipboardController {
    const MAX_CLIP_TRIES: usize = 5;

    pub fn read(&self) -> Option<String> {
        for _ in 0..Self::MAX_CLIP_TRIES {
            match self.0.clipboard().read() {
                Ok(clip) => match clip {
                    ClipboardContents::PlainText { text } => return Some(text),
                },
                Err(e) => {
                    println!("{e}");
                }
            }
        }
        None
    }

    pub fn write(&self, text: &str) -> bool {
        for _ in 0..Self::MAX_CLIP_TRIES {
            match self.0.clipboard().write(ClipKind::PlainText {
                label: None,
                text: text.to_string(),
            }) {
                Ok(_) => return true,
                Err(e) => {
                    println!("{e}");
                }
            }
        }

        false
    }
}

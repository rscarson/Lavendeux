use thiserror::Error;

#[derive(Debug, Error)]
pub enum Error {
    #[error("Could not lock the state")]
    Mutex,

    #[error("{0}")]
    Custom(String),

    #[error("{0}")]
    Parse(String),

    #[error("{0}")]
    JSON(#[from] serde_json::Error),

    #[error("{0}")]
    Tauri(#[from] tauri::Error),

    #[error("{0}")]
    Shortcut(#[from] tauri_plugin_global_shortcut::Error),

    #[error("{0}")]
    Clipboard(#[from] tauri_plugin_clipboard_manager::Error),

    #[error("{0}")]
    Autostart(#[from] tauri_plugin_autostart::Error),

    #[error("{}", .0.as_highlighted(Default::default()))]
    Javascript(#[from] lavendeux_js::Error),

    #[error("{0}")]
    IO(#[from] std::io::Error),
}

impl<T> From<std::sync::TryLockError<T>> for Error {
    fn from(_: std::sync::TryLockError<T>) -> Self {
        Error::Mutex
    }
}

impl From<Error> for tauri::ipc::InvokeError {
    fn from(e: Error) -> Self {
        tauri::ipc::InvokeError::from_error(e)
    }
}

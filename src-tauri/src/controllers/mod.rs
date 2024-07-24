use crate::error::Error;

pub trait Controller<T: Send + Sync> {
    const EVENT_NAME: &'static str;

    fn new_managed() -> Result<crate::ManagedValue<T>, Error>;

    fn state(&self) -> tauri::State<crate::ManagedValue<T>>;

    fn read(&self) -> Result<T, Error>;

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, T>, Error>;

    fn write(&self, value: &T) -> Result<T, Error>;

    fn emit(&self, value: &T) -> Result<(), Error>;
}

mod tray;
pub use tray::TrayController;

mod settings;
pub use settings::SettingsController;

mod shortcut;
pub use shortcut::ShortcutController;

mod language;
pub use language::LanguageController;

mod history;
pub use history::HistoryController;

mod clipboard;
pub use clipboard::ClipboardController;

mod extensions;
pub use extensions::ExtensionsController;

mod parser;
pub use parser::ParserController;

mod debug;
pub use debug::DebugController;

mod readibot;
pub use readibot::ReadibotController;

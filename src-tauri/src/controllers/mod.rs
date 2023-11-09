pub trait Controller<T: Send + Sync> {
    const EVENT_NAME: &'static str;

    fn new_managed() -> crate::ManagedValue<T>;

    fn state(&self) -> tauri::State<crate::ManagedValue<T>>;

    fn read(&self) -> Option<T>;

    fn borrow(&self) -> Option<std::sync::MutexGuard<'_, T>>;

    fn write(&self, value: &T) -> Result<T, String>;

    fn emit(&self, value: &T);
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

mod blacklist;
pub use blacklist::BlacklistController;

mod extensions;
pub use extensions::ExtensionsController;

mod parser;
pub use parser::ParserController;

mod debug;
pub use debug::{DebugController, DebugableResult};

mod config;
pub use config::ConfigController;

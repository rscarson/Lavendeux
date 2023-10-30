use tauri::AppHandle;
use tauri_plugin_dialog::DialogExt;

use crate::{models::settings, ManagedValue};

///
/// Unrecoverable error that required quitting the software
pub fn fatal(app: AppHandle, error: &str) -> Result<(), String> {
    app.dialog()
        .message(error)
        .ok_button_label("exit")
        .title("Fatal Error!")
        .show(move |_| {
            app.exit(1);
        });
    Err(error.to_string())
}

///
/// Normal error that may be shown to the user
pub fn parser(app: AppHandle, error: &str) {
    let settings = ManagedValue::<settings::Settings>::cloned_or_default(&app);

    if settings.display_errors {
        app.dialog()
            .message(error)
            .title("Parsing error!")
            .show(move |_| {});
    }
}

pub fn general(app: AppHandle, title: &str, error: &str) {
    app.dialog()
        .message(error)
        .ok_button_label("ok")
        .title(title)
        .show(move |_| {});
}

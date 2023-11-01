use tauri::AppHandle;
use tauri_plugin_dialog::DialogExt;
use tauri_plugin_notification::NotificationExt;

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
        app.notification()
            .builder()
            .title("Invalid expression!")
            .body(error)
            .show()
            .ok();
    }
}

pub fn general(app: AppHandle, title: &str, error: &str) {
    app.notification()
        .builder()
        .title(title)
        .body(error)
        .show()
        .ok();
}

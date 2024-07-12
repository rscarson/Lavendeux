use crate::controllers::{Controller, HistoryController, LanguageController, SettingsController};
use tauri::{
    menu::{CheckMenuItem, MenuBuilder, MenuItem},
    AppHandle,
};

// https://stackoverflow.com/questions/38461429/how-can-i-truncate-a-string-to-have-at-most-n-characters
fn truncate(s: &str, max_chars: usize) -> &str {
    match s.char_indices().nth(max_chars) {
        None => s,
        Some((idx, _)) => &s[..idx],
    }
}
pub trait MyMenuBuilder {
    fn add_separator(self) -> Self;
    fn add_item(self, app: &AppHandle, label: &str, id: &str, enabled: bool) -> Self;
    fn add_checkbox(
        self,
        app: &AppHandle,
        label: &str,
        id: &str,
        enabled: bool,
        checked: bool,
    ) -> Self;

    fn add_all(self, app: &AppHandle) -> Self;
}

impl<'m, R> MyMenuBuilder for MenuBuilder<'m, R, AppHandle>
where
    R: tauri::Runtime,
    AppHandle: tauri::Manager<R>,
{
    fn add_separator(self) -> Self {
        self.separator()
    }

    fn add_item(self, app: &AppHandle, label: &str, id: &str, enabled: bool) -> Self {
        self.item(&MenuItem::with_id(app, id, label, enabled, None::<&str>).unwrap())
    }

    fn add_checkbox(
        self,
        app: &AppHandle,
        label: &str,
        id: &str,
        enabled: bool,
        checked: bool,
    ) -> Self {
        self.item(&CheckMenuItem::with_id(app, id, label, enabled, checked, None::<&str>).unwrap())
    }

    fn add_all(self, app: &AppHandle) -> Self {
        let mut builder = self;
        let l = LanguageController(app.clone());
        let settings = SettingsController(app.clone()).read().unwrap_or_default();
        let history = HistoryController(app.clone()).read().unwrap_or_default();

        if history.is_empty() {
            //
            // No history to add - use placeholder
            //
            builder = builder.add_item(
                app,
                &l.translate_or_default("history\\lbl_no_history_to_display"),
                "empty_history",
                false,
            );
        } else {
            //
            // Add all history items
            //
            for snippet in history.iter().filter(|e| !e.is_err()) {
                let mut text =
                    truncate(snippet.input.lines().next().unwrap_or_default(), 35).to_string();
                if snippet.input.len() != text.len() {
                    text += " [...]";
                }
                builder = builder.add_item(app, &text, &snippet.input, true);
            }
        }

        builder = builder.add_separator();

        builder = builder.add_item(
            app,
            &l.translate_or_default("tray\\lbl_history"),
            "history",
            true,
        );
        builder = builder.add_item(
            app,
            &l.translate_or_default("tray\\lbl_settings"),
            "settings",
            true,
        );

        builder = builder.add_separator();

        builder = builder.add_checkbox(
            app,
            &l.translate_or_default("tray\\lbl_ecm"),
            "enhanced_clipboard",
            true,
            settings.enhanced_clipboard,
        );
        builder = builder.add_checkbox(
            app,
            &l.translate_or_default("tray\\lbl_errors"),
            "display_errors",
            true,
            settings.display_errors,
        );

        builder = builder.add_separator();
        builder.add_item(app, &l.translate_or_default("tray\\lbl_quit"), "quit", true)
    }
}

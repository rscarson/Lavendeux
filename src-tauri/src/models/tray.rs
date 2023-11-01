use tauri::{
    menu::{CheckMenuItem, MenuBuilder, MenuEvent, MenuItem},
    tray::TrayIconBuilder,
    AppHandle, Manager,
};
use tauri_plugin_clipboard_manager::{ClipKind, ClipboardExt};

use crate::managed_value::ManagedValue;

use super::{
    language::{self, TranslatorManager},
    settings, snippet,
};

pub fn init_tray(app: &AppHandle) -> Result<(), tauri::Error> {
    TrayIconBuilder::new()
        .on_menu_event(handle_menu_event)
        .on_tray_icon_event(|i, e| println!("here"))
        .build(app)?;
    update_tray(app)
}

fn handle_menu_event(app: &AppHandle, event: MenuEvent) {
    let settings = settings::read_settings(app.state::<ManagedValue<settings::Settings>>());
    if settings.is_err() {
        return;
    }
    let mut settings = settings.unwrap();

    match event.id().0.as_str() {
        "Settings" => {
            if let Some(window) = app.get_window("main") {
                app.emit("tab-request", "/settings").ok();
                window.show().ok();
                window.set_focus().ok();
            }
        }
        "History" => {
            if let Some(window) = app.get_window("main") {
                app.emit("tab-request", "/history").ok();
                window.show().ok();
                window.set_focus().ok();
            }
        }
        "enhanced_clipboard" => {
            settings.enhanced_clipboard = !settings.enhanced_clipboard;
            settings::write_settings(
                settings.clone(),
                app.state::<ManagedValue<settings::Settings>>(),
                app.clone(),
            )
            .ok();
            app.emit("updated-settings", settings.clone()).ok();
        }
        "display_errors" => {
            settings.display_errors = !settings.display_errors;
            settings::write_settings(
                settings.clone(),
                app.state::<ManagedValue<settings::Settings>>(),
                app.clone(),
            )
            .ok();
            app.emit("updated-settings", settings.clone()).ok();
        }
        "Quit" => {
            app.exit(0);
        }
        s => {
            // Assume it's an expression and copy it
            app.clipboard()
                .write(ClipKind::PlainText {
                    label: None,
                    text: s.to_string(),
                })
                .ok();
        }
    }
}

// https://stackoverflow.com/questions/38461429/how-can-i-truncate-a-string-to-have-at-most-n-characters
fn truncate(s: &str, max_chars: usize) -> &str {
    match s.char_indices().nth(max_chars) {
        None => s,
        Some((idx, _)) => &s[..idx],
    }
}

pub fn update_tray(app: &AppHandle) -> Result<(), tauri::Error> {
    let translate = |path: &str| {
        TranslatorManager::translate(
            path.to_string(),
            &app.clone().state::<language::ManagedTranslator>(),
        )
        .unwrap_or_default()
    };

    let settings = settings::read_settings(app.state::<ManagedValue<settings::Settings>>());
    if settings.is_err() {
        return Err(tauri::Error::AssetNotFound("".to_string()));
    }
    let settings = settings.unwrap();

    let history = snippet::read_history(app.state::<ManagedValue<snippet::History>>());
    if history.is_none() {
        return Err(tauri::Error::AssetNotFound("".to_string()));
    }
    let history = history.unwrap();

    if let Some(tray) = app.tray() {
        let mut builder = MenuBuilder::new(app);

        let mut n = 0;
        for snippet in history {
            if snippet.is_err() {
                continue;
            }
            n += 1;

            let mut text =
                truncate(snippet.input.lines().next().unwrap_or_default(), 35).to_string();
            if snippet.input.len() != text.len() {
                text += " [...]";
            }

            builder = builder.item(&MenuItem::with_id(app, &snippet.input, &text, true, None));
        }

        if 0 == n {
            builder = builder.item(&MenuItem::new(app, "No history to display", false, None));
        }

        let menu = builder
            .separator()
            .item(&MenuItem::with_id(
                app,
                "History",
                translate("tray\\lbl_history"),
                true,
                None,
            ))
            .item(&MenuItem::with_id(
                app,
                "Settings",
                translate("tray\\lbl_settings"),
                true,
                None,
            ))
            .item(&CheckMenuItem::with_id(
                app,
                "enhanced_clipboard",
                translate("tray\\lbl_ecm"),
                true,
                settings.enhanced_clipboard,
                None,
            ))
            .item(&CheckMenuItem::with_id(
                app,
                "display_errors",
                translate("tray\\lbl_errors"),
                true,
                settings.display_errors,
                None,
            ))
            .separator()
            .item(&MenuItem::with_id(
                app,
                "Quit",
                translate("tray\\lbl_quit"),
                true,
                None,
            ))
            .build()?;
        tray.set_menu(Some(menu))
    } else {
        Err(tauri::Error::AssetNotFound("".to_string()))
    }
}

use serde::{Deserialize, Serialize};

mod en;

#[derive(Serialize, Deserialize, Clone)]
pub struct Language {
    pub logview_label_timestamp: &'static str,
    pub logview_label_level: &'static str,
    pub logview_label_event: &'static str,
    pub logview_empty: &'static str,
    pub logview_btn_clear: &'static str,
    pub logview_btn_open: &'static str,

    pub errorview_parsing_error: &'static str,

    pub mainview_history: &'static str,
    pub mainview_extensions: &'static str,
    pub mainview_settings: &'static str,
    pub mainview_help: &'static str,

    pub historyview_btn_clear: &'static str,
    pub historyview_no_history: &'static str,
    pub historyview_getting_started_highlight: &'static str,
    pub historyview_getting_started_copy: &'static str,

    pub extensionview_btn_disable: &'static str,
    pub extensionview_btn_import: &'static str,
    pub extensionview_btn_reload: &'static str,
    pub extensionview_btn_open: &'static str,
    pub extensionview_no_extensions: &'static str,
    pub extensionview_functions: &'static str,
    pub extensionview_decorators: &'static str,
    pub extensionview_extension: &'static str,
    pub extensionview_about: &'static str,
    pub extensionview_author: &'static str,

    pub settingview_clipboard_mode: &'static str,
    pub settingview_clipboard_mode_desc: &'static str,
    pub settingview_clipboard_mode_auto: &'static str,
    pub settingview_clipboard_mode_off: &'static str,

    pub settingview_silent_errors: &'static str,
    pub settingview_silent_errors_desc: &'static str,
    pub settingview_silent_errors_on: &'static str,
    pub settingview_silent_errors_off: &'static str,

    pub settingview_autostart: &'static str,
    pub settingview_autostart_desc: &'static str,
    pub settingview_autostart_on: &'static str,
    pub settingview_autostart_off: &'static str,

    pub settingview_theme: &'static str,
    pub settingview_theme_desc: &'static str,
    pub settingview_theme_on: &'static str,
    pub settingview_theme_off: &'static str,

    pub settingview_keyboard_shortcut: &'static str,
    pub settingview_keyboard_shortcut_desc: &'static str,

    pub settingview_extension_dir: &'static str,
    pub settingview_extension_dir_desc: &'static str,

    pub settingview_save: &'static str,

    pub helpview_help_1: &'static str,
    pub helpview_help_2: &'static str,
    pub helpview_help_3: &'static str,

    pub menu_file: &'static str,
    pub menu_file_quit: &'static str,
    pub menu_help: &'static str,
    pub menu_help_about: &'static str,
    pub menu_help_logs: &'static str,
    pub menu_help_help: &'static str,

    pub logviewer_title: &'static str
}

impl Language {
    fn en() -> Language {
        en::lang()
    }
}

#[tauri::command]
pub fn lang_en() -> Language {
    Language::en()
}
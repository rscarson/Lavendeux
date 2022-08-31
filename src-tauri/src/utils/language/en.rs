use super::Language;

pub fn lang() -> Language {
    Language {
        logview_label_timestamp: "Timestamp",
        logview_label_level: "Level",
        logview_label_event: "Event",
        logview_empty: "Nothing to display",
        logview_btn_clear: "Clear Log",
        logview_btn_open: "Open Log Directory",
    
        errorview_parsing_error: "Could not parse expression",

        mainview_history: "History",
        mainview_extensions: "Extensions",
        mainview_settings: "Settings",
        mainview_help: "Help",
    
        historyview_no_history: "No history to display.",
        historyview_btn_clear: "Clear history",
        historyview_getting_started_highlight: "To get started, try highlighting the following block of text, and pressing",
        historyview_getting_started_copy: "To get started, try copying the following block of text, and pressing",
    
        extensionview_btn_disable: "Disable",
        extensionview_btn_import: "Import Extensions",
        extensionview_btn_reload: "Reload Extensions",
        extensionview_btn_open: "Open Extensions Directory",
        extensionview_no_extensions: "No extensions installed",
        extensionview_functions: "functions",
        extensionview_decorators: "decorators",
        extensionview_extension: "Extension",
        extensionview_about: "About",
        extensionview_author: "Author",
    
        settingview_clipboard_mode: "Clipboard Mode",
        settingview_clipboard_mode_desc: "Determines where text is pulled from for parsing",
        settingview_clipboard_mode_auto: "Replace highlighted text, bypassing the clipboard",
        settingview_clipboard_mode_off: "Replace clipboard contents",
    
        settingview_silent_errors: "Silent Errors",
        settingview_silent_errors_desc: "Suppresses the popup message on parsing errors",
        settingview_silent_errors_on: "Log errors, but do not display them",
        settingview_silent_errors_off: "Display all parsing errors",
    
        settingview_autostart: "Start Automatically",
        settingview_autostart_desc: "Opens Lavendeux automatically when logging in to your computer",
        settingview_autostart_on: "Start Lavendeux when logging in",
        settingview_autostart_off: "Do not start Lavendeux automatically",
    
        settingview_theme: "Theme",
        settingview_theme_desc: "Visual style for the settings window",
        settingview_theme_on: "Dark theme",
        settingview_theme_off: "Light theme",
    
        settingview_keyboard_shortcut: "Keyboard shortcut",
        settingview_keyboard_shortcut_desc: "The shortcut that parses the clipboard contents",
    
        settingview_extension_dir: "Extension directory",
        settingview_extension_dir_desc: "Imported extensions will be copied to this directory",

        
        settingview_language: "Language",
        settingview_language_desc: "Language settings",
    
        settingview_save: "Save Changes",
    
        helpview_help_1: "In Windows, by default the keyboard shortcut",
        helpview_help_2: "will interpret any currently highlighted text as an expression, parse that expression, and replace it with the resulting value.",
        helpview_help_3: "On other platforms, or when the clipboard mode has been changed, then instead of using highlighted text, Lavendeux will use the contents of the clipboard.",

        menu_file: "File",
        menu_file_quit: "Quit",
        menu_help: "Help",
        menu_help_about: "About",
        menu_help_logs: "Logs",
        menu_help_help: "Help",
        menu_tray_settings: "Settings",
        menu_tray_exit: "Exit",

        logviewer_title: "Log Viewer"
    }
}
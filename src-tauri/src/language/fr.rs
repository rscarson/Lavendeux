use crate::language::Language;

pub fn lang() -> Language {
    Language {
        logview_label_timestamp: "F Timestamp",
        logview_label_level: "F Level",
        logview_label_event: "F Event",
        logview_empty: "F Nothing to display",
        logview_btn_clear: "F Clear Log",
        logview_btn_open: "F Open Log Directory",
    
        errorview_parsing_error: "F Could not parse expression",

        mainview_history: "F History",
        mainview_extensions: "F Extensions",
        mainview_settings: "F Settings",
        mainview_help: "F Help",
    
        historyview_no_history: "F No history to display.",
        historyview_btn_clear: "F Clear history",
        historyview_getting_started_highlight: "F To get started, try highlighting the following block of text, and pressing",
        historyview_getting_started_copy: "F To get started, try copying the following block of text, and pressing",
    
        extensionview_btn_disable: "F Disable",
        extensionview_btn_import: "F Import Extensions",
        extensionview_btn_reload: "F Reload Extensions",
        extensionview_btn_open: "F Open Extensions Directory",
        extensionview_no_extensions: "F No extensions installed",
        extensionview_functions: "F functions",
        extensionview_decorators: "F decorators",
        extensionview_extension: "F Extension",
        extensionview_about: "F About",
        extensionview_author: "F Author",
    
        settingview_clipboard_mode: "F Clipboard Mode",
        settingview_clipboard_mode_desc: "F Determines where text is pulled from for parsing",
        settingview_clipboard_mode_auto: "F Replace highlighted text, bypassing the clipboard",
        settingview_clipboard_mode_off: "F Replace clipboard contents",
    
        settingview_silent_errors: "F Silent Errors",
        settingview_silent_errors_desc: "F Suppresses the popup message on parsing errors",
        settingview_silent_errors_on: "F Log errors, but do not display them",
        settingview_silent_errors_off: "F Display all parsing errors",
    
        settingview_autostart: "F Start Automatically",
        settingview_autostart_desc: "F Opens Lavendeux automatically when logging in to your computer",
        settingview_autostart_on: "F Start Lavendeux when logging in",
        settingview_autostart_off: "F Do not start Lavendeux automatically",
    
        settingview_theme: "F Theme",
        settingview_theme_desc: "F Visual style for the settings window",
        settingview_theme_on: "F Dark theme",
        settingview_theme_off: "F Light theme",
    
        settingview_keyboard_shortcut: "F Keyboard shortcut",
        settingview_keyboard_shortcut_desc: "F The shortcut that parses the clipboard contents",
    
        settingview_extension_dir: "F Extension directory",
        settingview_extension_dir_desc: "F Imported extensions will be copied to this directory",

        
        settingview_language: "F Language",
        settingview_language_desc: "F Language settings",
    
        settingview_save: "F Save Changes",
    
        helpview_help_1: "F In Windows, by default the keyboard shortcut",
        helpview_help_2: "F will interpret any currently highlighted text as an expression, parse that expression, and replace it with the resulting value.",
        helpview_help_3: "F On other platforms, or when the clipboard mode has been changed, then instead of using highlighted text, Lavendeux will use the contents of the clipboard.",

        menu_file: "F File",
        menu_file_quit: "F Quit",
        menu_help: "F Help",
        menu_help_about: "F About",
        menu_help_logs: "F Logs",
        menu_help_help: "F Help",
        menu_tray_settings: "F Settings",
        menu_tray_exit: "F Exit",

        logviewer_title: "F Log Viewer"
    }
}
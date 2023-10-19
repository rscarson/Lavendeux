export interface KeyboardShortcut {
    key: string;
    ctrl: boolean;
    alt: boolean;
    shift: boolean;
}

export interface Settings {
    enhanced_clipboard: boolean,
    silence_errors: boolean,
    start_with_os: boolean,

    start_script: string,
    shortcut: KeyboardShortcut,

    dark_theme: boolean,
    language_code: string,
}

export type Nullable<T> = T | null;
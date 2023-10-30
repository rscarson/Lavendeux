export interface KeyboardShortcut {
    key: string;
    ctrl: boolean;
    alt: boolean;
    shift: boolean;
}

export interface ExtensionFunction {
    returns: string,
    argument_types: Array<string>,
    fname: string,
    ftype: string,
};

export interface Extension {
    Ok?: {
        name: string,
        author: string,
        version: string,

        functions: Map<String, ExtensionFunction | string>,
        decorators: Map<String, ExtensionFunction | string>,
    },
    Err?: string
}

export interface Snippet {
    input: string,
    result: {
        Success?: string,
        Error?:string
    }
    timestamp: {
        nanos_since_epoch: number,
        secs_since_epoch: number
    };
}

export interface Settings {
    enhanced_clipboard: boolean,
    display_errors: boolean,
    start_with_os: boolean,

    start_script: string,
    shortcut: KeyboardShortcut,

    dark_theme: boolean,
    language_code: string
}

export type Nullable<T> = T | null;
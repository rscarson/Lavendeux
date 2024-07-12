export interface KeyboardShortcut {
    key: string;
    ctrl: boolean;
    alt: boolean;
    shift: boolean;
}

export interface FunctionDocs {
    name: string,
    category: string,
    signature: string,
    short: Nullable<string>,
    description: Nullable<string>,
    examples: Nullable<string>,
};

export interface Extension {
    id: number,
    module: Module,
    metadata: ExtensionMetadata,
    functions: Array<FunctionDocs>,
    problem: Nullable<string>,
}

export interface ExtensionMetadata {
    
    name: Nullable<string>,
    version: Nullable<string>,
    description: Nullable<string>,
    author: Nullable<string>,
    license: Nullable<string>,
}

export interface Module {
    filename: string,
    contents: string,
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
    timeout_ms: number,
    shortcut: KeyboardShortcut,

    dark_theme: boolean,
    language_code: string,
}

export interface MarkdownToken {
    H4?: string,
    H3?: string,
    H2?: string,
    H1?: string,
    HR,
    CodeBlock?: Array<string>,
    UL?: Array<string>,
    PlainText?: string,
}

export interface MarkdownTree {
    heading: string,
    contents: Array<MarkdownToken>,
    subheadings: Map<string, Array<MarkdownToken>>,
}

export type Nullable<T> = T | null;
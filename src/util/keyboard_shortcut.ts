import { KeyboardShortcut, Nullable } from '../types';

export class KeyboardShortcutController {
    static EVENT_NAME = "lavendeux_recordkeyshortcut";

    static _recordKeydownEvent(e: KeyboardEvent) {
        e.preventDefault();

        let code = e.code.replace(/Left|Right|Key/g, '');
        if (['Control', 'Shift', 'Alt'].includes(code)) return;

        let key = {
            key: e.code.replace(/Left|Right|Key/g, ''),
            ctrl: e.ctrlKey,
            alt: e.altKey,
            shift: e.shiftKey
        };

        var event = new CustomEvent<KeyboardShortcut>(KeyboardShortcutController.EVENT_NAME, {detail: key});
        document.dispatchEvent(event);
    }

    static stopRecording() {
        document.removeEventListener("keydown", KeyboardShortcutController._recordKeydownEvent);
    }

    static record():Promise<KeyboardShortcut> {
        return new Promise((resolve, reject) => {
            document.addEventListener("keydown", KeyboardShortcutController._recordKeydownEvent);
            document.addEventListener(KeyboardShortcutController.EVENT_NAME, ((e: CustomEvent<KeyboardShortcut>) => {
                KeyboardShortcutController.stopRecording();
                resolve(e.detail);
            }) as EventListener);
        });
    }

    static toString(shortcut: KeyboardShortcut): string {
        let str = [
            (shortcut.ctrl && 'CTRL'),
            (shortcut.alt && 'ALT'),
            (shortcut.shift && 'SHIFT'),
            (shortcut.key),
        ].filter((m)=>m).join('+');
        return (str=='') ? 'None' : str;
    }
}
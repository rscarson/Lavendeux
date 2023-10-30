use std::{thread, time::Duration};

use enigo::{Enigo, Key, KeyboardControllable};
// use enigo::{EnigoSettings, Key, KeyboardControllable};
use lavendeux_parser::{ParserState, Token};
use tauri::{AppHandle, Manager};
use tauri_plugin_clipboard_manager::{ClipKind, ClipboardContents, ClipboardExt};

use crate::{bugcheck, ManagedValue};

use super::{
    settings,
    shortcut::Shortcut,
    snippet::{append_history, Snippet, SnippetResult},
};

const MAX_CLIP_TRIES: usize = 5;
fn read_clip(app: &AppHandle) -> Option<String> {
    for _ in 0..MAX_CLIP_TRIES {
        match app.clipboard().read() {
            Ok(clip) => match clip {
                ClipboardContents::PlainText(s) => return Some(s),
            },
            Err(_) => {}
        }
    }
    None
}

fn write_clip(app: &AppHandle, text: &str) -> Result<(), ()> {
    for _ in 0..MAX_CLIP_TRIES {
        match app.clipboard().write(ClipKind::PlainText {
            label: None,
            text: text.to_string(),
        }) {
            Ok(_) => return Ok(()),
            Err(_) => {}
        }
    }

    println!("Warning: clipboard writeout failure");
    Err(())
}

fn send_sequence(modifier: Key, key: char, configured_shortcut: &Shortcut) {
    let mut enigo = Enigo::new();

    // Reset modifiers
    enigo.key_up(Key::Control);
    enigo.key_up(Key::Alt);
    enigo.key_up(Key::Shift);
    enigo.key_up(configured_shortcut.to_enigo_key().unwrap());

    enigo.key_down(modifier);
    enigo.key_click(Key::Layout(key));
    enigo.key_up(modifier);
}

/// Send a CTRL-C key sequence to the system
fn send_copy(configured_shortcut: &Shortcut) {
    send_sequence(Key::Control, 'c', configured_shortcut);
}

/// Send a CTRL-V key sequence to the system
fn send_paste(configured_shortcut: &Shortcut) {
    send_sequence(Key::Control, 'v', configured_shortcut);
}

/// Spawns a thread to handle the hotkey
fn spawn_handler<F>(app_handle: AppHandle, handler: F)
where
    F: Fn(AppHandle) + Send + Sync + 'static,
{
    thread::spawn(move || handler(app_handle));
}

pub fn handle_event(app: AppHandle) {
    // Check if we are using ECM
    let settings = settings::read_settings(app.state::<ManagedValue<settings::Settings>>());
    if settings.is_err() {
        bugcheck::parser(app.clone(), "Could not lock the clipboard");
        return;
    }
    let settings = settings.unwrap();

    // Backup the clipboard
    let clip_backup = read_clip(&app);
    if clip_backup.is_none() {
        bugcheck::parser(app.clone(), "Could not lock the clipboard");
        return;
    }
    let clip_backup = clip_backup.unwrap();

    // Copy, if need be
    if settings.enhanced_clipboard {
        send_copy(&settings.shortcut);
        thread::sleep(Duration::from_millis(10));
    }

    if let Some(input) = read_clip(&app) {
        let snippet = parse(input, app.clone());
        if !snippet.is_err() {
            println!("writing result...");
            write_clip(&app, &snippet.result.to_string());
            if settings.enhanced_clipboard {
                println!("sending paste...");
                thread::sleep(Duration::from_millis(10));
                send_paste(&settings.shortcut);
            }
        }

        // Fix the clipboard if need be
        if settings.enhanced_clipboard {
            thread::sleep(Duration::from_millis(500));
            println!("restoring clipboard...");
            write_clip(&app, &clip_backup);
        }
    } else {
        bugcheck::parser(app.clone(), "Could not lock the clipboard");
        write_clip(&app, &clip_backup);
        return;
    }
}

pub fn parse(input: String, app: AppHandle) -> Snippet {
    // Unwrap the parser state
    let parser_state = app.state::<ManagedValue<ParserState>>();
    let parser_state = parser_state.read();
    if parser_state.is_none() {
        todo!("error management");
    }
    let mut parser_state = parser_state.unwrap();

    let snippet = match Token::new(&input, &mut parser_state) {
        Ok(token) => Snippet::new(&input, SnippetResult::Success(token.to_string())),
        Err(e) => Snippet::new(&input, SnippetResult::Error(e.to_string())),
    };

    if snippet.is_err() {
        bugcheck::parser(app.clone(), &snippet.result.to_string());
    }

    // Write to history
    append_history(snippet.clone(), app.clone());
    snippet
}

pub fn shortcut_handler(app: &AppHandle, _shortcut: &tauri_plugin_global_shortcut::Shortcut) {
    spawn_handler(app.clone(), handle_event);
}

use std::{thread, time::Duration};

use lavendeux_parser2::{Lavendeux, ParserOptions};
use tauri::{AppHandle, Manager, State};

use crate::{
    bugcheck, debug,
    managed_value::ManagedValue,
    models::history::{Snippet, SnippetResult},
};

use super::{
    ClipboardController, Controller, DebugableResult, HistoryController, SettingsController,
    ShortcutController,
};

pub struct ParserController(pub AppHandle);
impl Controller<Lavendeux> for ParserController {
    const EVENT_NAME: &'static str = "updated-parserstate";

    fn new_managed() -> ManagedValue<Lavendeux> {
        ManagedValue::new(Lavendeux::new(ParserOptions {
            timeout: 2,
            stack_size: 10 * 1024 * 1024,
        }))
    }

    fn state(&self) -> State<ManagedValue<Lavendeux>> {
        self.0.state::<ManagedValue<Lavendeux>>()
    }

    fn read(&self) -> Option<Lavendeux> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Option<std::sync::MutexGuard<'_, Lavendeux>> {
        self.state().inner().read()
    }

    fn write(&self, value: &Lavendeux) -> Result<Lavendeux, String> {
        self.state()
            .write(value.clone())
            .or(Err("could not write to state".to_string()))?;
        self.emit(&value);

        Ok(value.clone())
    }

    fn emit(&self, _: &Lavendeux) {
        self.0
            .emit(Self::EVENT_NAME, ())
            .debug_ok(&self.0, "emit-event");
    }
}

impl ParserController {
    ///
    /// Parse an input using the active parser
    pub fn parse(&self, input: &str) -> Snippet {
        if let Some(mut parser) = self.borrow() {
            let snippet = match parser.parse(input) {
                Ok(values) => Snippet::new(
                    &input,
                    SnippetResult::Success(
                        values
                            .iter()
                            .map(|v| v.to_string())
                            .collect::<Vec<_>>()
                            .join("\n"),
                    ),
                ),
                Err(e) => Snippet::new(&input, SnippetResult::Error(e.to_string())),
            };

            HistoryController(self.0.clone()).add(snippet.clone());
            snippet
        } else {
            Snippet::new(
                &input,
                SnippetResult::Error("could not lock parser state".to_string()),
            )
        }
    }

    fn handle_shortcut(&self) -> Result<(), String> {
        debug!(self.0.clone(), "Handling keyboard event");
        // Check if we are using ECM
        let is_ecm = SettingsController(self.0.clone())
            .read()
            .unwrap_or_default()
            .enhanced_clipboard;

        // ECM processing start - back up the clipboard for eventual restore
        let ecm_backup = if is_ecm {
            let s = ClipboardController(self.0.clone())
                .read()
                .ok_or("could not lock clipboard for reading".to_string())?;

            // Send copy at the same time to get the real input into the clipboard now
            // We sleep a bit to ensure it has finished the copy-out process
            ShortcutController(self.0.clone()).send_copy();
            thread::sleep(Duration::from_millis(10));

            s
        } else {
            "".to_string()
        };

        // Get our snippet and process it
        let input = ClipboardController(self.0.clone())
            .read()
            .ok_or("could not lock clipboard for reading".to_string())?;
        let snippet = self.parse(&input);

        // If all went well, we overwrite the clipboard contents here
        if !snippet.is_err() {
            if !ClipboardController(self.0.clone()).write(&snippet.result.to_string()) {
                return Err("could not lock clipboard for writing".to_string());
            }
        }

        // ECM processing end - paste out the result and restore the clipboard state
        if is_ecm {
            if !snippet.is_err() {
                thread::sleep(Duration::from_millis(10));
                ShortcutController(self.0.clone()).send_paste();
            }

            // This wait is to accomodate the larger paste event above
            thread::sleep(Duration::from_millis(500));
            if !ClipboardController(self.0.clone()).write(&ecm_backup) {
                return Err("could not lock clipboard for writing".to_string());
            }
        }

        if snippet.is_err() {
            Err(snippet.result.to_string())
        } else {
            Ok(())
        }
    }

    ///
    /// Threaded main handler for input events
    pub fn main_handler(app: AppHandle) {
        thread::spawn(move || {
            if let Err(e) = ParserController(app.clone()).handle_shortcut() {
                bugcheck::parser(app, &e);
            }
        });
    }
}

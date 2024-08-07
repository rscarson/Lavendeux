use crate::{
    bugcheck,
    controllers::ExtensionsController,
    debug,
    error::Error,
    managed_value::ManagedValue,
    models::history::{Snippet, SnippetResult},
};
use std::{thread, time::Duration};
use tauri::{AppHandle, Manager, State};
use tauri_plugin_global_shortcut::ShortcutEvent;

use super::{
    ClipboardController, Controller, HistoryController, SettingsController, ShortcutController,
};

pub struct ParserController(pub AppHandle);
impl Controller<()> for ParserController {
    const EVENT_NAME: &'static str = "updated-parserstate";

    fn new_managed() -> Result<ManagedValue<()>, Error> {
        lavendeux_js::init_worker(std::time::Duration::from_secs(3))?;
        Ok(ManagedValue::new(()))
    }

    fn state(&self) -> State<ManagedValue<()>> {
        self.0.state::<ManagedValue<()>>()
    }

    fn read(&self) -> Result<(), Error> {
        Ok(())
    }

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, ()>, Error> {
        self.state().inner().read()
    }

    fn write(&self, value: &()) -> Result<(), Error> {
        self.state().write(())?;
        self.emit(value)?;

        Ok(())
    }

    fn emit(&self, _: &()) -> Result<(), Error> {
        self.0.emit(Self::EVENT_NAME, ())?;
        Ok(())
    }
}

impl ParserController {
    ///
    /// Restart the parser
    pub fn restart_parser(&self) -> Result<(), Error> {
        debug!(self.0.clone(), "Restarting parser");
        lavendeux_js::with_worker(|runtime| runtime.restart())?;
        ExtensionsController(self.0.clone()).load()
    }

    ///
    /// Parse an input using the active parser
    pub fn parse(&self, input: &str) -> Result<Snippet, Error> {
        let result = lavendeux_js::with_worker(|runtime| runtime.eval(input));
        let snippet = match result {
            Ok(s) => Snippet::new(input, SnippetResult::Success(s.to_string())),
            Err(e) => {
                let mut str = e.as_highlighted(Default::default());
                str = str
                    .replace("\n  at :", "\n  at ")
                    .replace("\n  at <anonymous>:", "\n  at ");

                Snippet::new(input, SnippetResult::Error(str))
            }
        };

        HistoryController(self.0.clone()).add(snippet.clone())?;
        Ok(snippet)
    }

    fn handle_shortcut(&self) -> Result<(), Error> {
        debug!(self.0.clone(), "Handling keyboard event");

        // Check if we are using ECM
        let is_ecm = SettingsController(self.0.clone())
            .read()
            .unwrap_or_default()
            .enhanced_clipboard;

        // ECM processing start - back up the clipboard for eventual restore
        let ecm_backup = if is_ecm {
            let s = ClipboardController(self.0.clone()).read()?;

            // Send copy at the same time to get the real input into the clipboard now
            // We sleep a bit to ensure it has finished the copy-out process
            ShortcutController(self.0.clone()).send_copy();
            thread::sleep(Duration::from_millis(10));

            Some(s)
        } else {
            None
        };

        // Get our snippet and process it
        let input = ClipboardController(self.0.clone()).read()?;
        let snippet = self.parse(&input)?;

        // If all went well, we overwrite the clipboard contents here
        if !snippet.is_err() {
            ClipboardController(self.0.clone()).write(&snippet.result.to_string())?;
        }

        // ECM processing end - paste out the result and restore the clipboard state
        if let Some(ecm_backup) = ecm_backup {
            if !snippet.is_err() {
                thread::sleep(Duration::from_millis(10));
                ShortcutController(self.0.clone()).send_paste();
            }

            // This wait is to accomodate the larger paste event above
            thread::sleep(Duration::from_millis(500));
            ClipboardController(self.0.clone()).write(&ecm_backup)?;
        }

        if snippet.is_err() {
            Err(Error::Parse(snippet.result.to_string()))
        } else {
            Ok(())
        }
    }

    ///
    /// Threaded main handler for input events
    pub fn main_handler(app: AppHandle, _: ShortcutEvent) {
        thread::spawn(move || {
            if let Err(e) = ParserController(app.clone()).handle_shortcut() {
                bugcheck::parser(app, &e.to_string())
            }
        });
    }
}

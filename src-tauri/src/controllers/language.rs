use crate::{debug, managed_value::ManagedValue, models::markdown::MarkdownTree};

use super::{Controller, DebugableResult};
use embedded_lang::{embedded_language, LanguageSet};
use tauri::{AppHandle, Manager, State};

pub struct LanguageController(pub AppHandle);
impl Controller<LanguageSet> for LanguageController {
    const EVENT_NAME: &'static str = "language-updated";

    fn new_managed() -> Result<crate::ManagedValue<LanguageSet>, String> {
        let mut translator = LanguageSet::new(
            "en",
            &[
                embedded_language!("../../../language/en.lang.json"),
                embedded_language!("../../../language/fr.lang.json"),
                embedded_language!("../../../language/de.lang.json"),
                embedded_language!("../../../language/ja.lang.json"),
                embedded_language!("../../../language/zh-cn.lang.json"),
            ],
        );
        translator.set_fallback_language("en");
        translator.attach(
            "en",
            "help",
            MarkdownTree::parse(include_str!("../../../language/help/en.help.md")),
        );

        Ok(ManagedValue::new(translator))
    }

    fn state(&self) -> State<ManagedValue<LanguageSet>> {
        self.0.state::<ManagedValue<LanguageSet>>()
    }

    fn read(&self) -> Option<LanguageSet> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Option<std::sync::MutexGuard<'_, LanguageSet>> {
        self.state().inner().read()
    }

    fn write(&self, value: &LanguageSet) -> Result<LanguageSet, String> {
        self.state()
            .write(value.clone())
            .or(Err("could not write to state".to_string()))?;
        self.emit(value);
        Ok(value.clone())
    }

    fn emit(&self, value: &LanguageSet) {
        self.0
            .emit(Self::EVENT_NAME, value.clone())
            .debug_ok(&self.0, "emit-event");
    }
}

impl LanguageController {
    pub fn translate(&self, path: &str) -> Option<String> {
        self.borrow()
            .and_then(|t| t.get(path).and_then(|s| Some(s.to_string())))
    }

    pub fn translate_or_default(&self, path: &str) -> String {
        self.translate(path).unwrap_or_default()
    }

    pub fn help_text(&self) -> Option<MarkdownTree> {
        match self.borrow() {
            Some(t) => t.attachment("help"),
            None => {
                println!("No help text found");
                None
            }
        }
    }

    pub fn set_language(&self, lang: &str) -> Result<(), String> {
        if let Some(mut translator) = self.borrow() {
            if translator.set_language(lang) {
                debug!(self.0.clone(), "Language now set to {}", lang);
                self.emit(&translator);
                return Ok(());
            }
        }

        Err("could not set language".to_string())
    }

    pub fn list(&self) -> Vec<(String, String)> {
        if let Some(translator) = self.borrow() {
            translator
                .all_languages()
                .iter()
                .map(|l| (l.name().to_string(), l.short_name().to_string()))
                .collect()
        } else {
            Vec::default()
        }
    }
}

use super::Controller;
use crate::{debug, error::Error, managed_value::ManagedValue, models::markdown::MarkdownTree};
use embedded_lang::{embedded_language, LanguageSet};
use tauri::{AppHandle, Manager, State};

pub struct LanguageController(pub AppHandle);
impl Controller<LanguageSet> for LanguageController {
    const EVENT_NAME: &'static str = "language-updated";

    fn new_managed() -> Result<crate::ManagedValue<LanguageSet>, Error> {
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

    fn read(&self) -> Result<LanguageSet, Error> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, LanguageSet>, Error> {
        self.state().inner().read()
    }

    fn write(&self, value: &LanguageSet) -> Result<LanguageSet, Error> {
        self.state().write(value.clone())?;
        self.emit(value)?;
        Ok(value.clone())
    }

    fn emit(&self, value: &LanguageSet) -> Result<(), Error> {
        self.0.emit(Self::EVENT_NAME, value.clone())?;
        Ok(())
    }
}

impl LanguageController {
    pub fn translate(&self, path: &str) -> Result<String, Error> {
        match self.borrow()?.get(path) {
            Some(s) => Ok(s.to_string()),
            None => Err(Error::Custom(format!("No translation found for {}", path))),
        }
    }

    pub fn translate_or_default(&self, path: &str) -> String {
        self.translate(path).unwrap_or_default()
    }

    pub fn help_text(&self) -> Result<MarkdownTree, Error> {
        self.borrow()?
            .attachment("help")
            .ok_or(Error::Custom("No help text found".to_string()))
    }

    pub fn set_language(&self, lang: &str) -> Result<(), Error> {
        let mut translator = self.borrow()?;
        if translator.set_language(lang) {
            debug!(self.0.clone(), "Language now set to {}", lang);
            self.emit(&translator)?;
            Ok(())
        } else {
            Err(Error::Custom("could not set language".to_string()))
        }
    }

    pub fn list(&self) -> Result<Vec<(String, String)>, Error> {
        Ok(self
            .borrow()?
            .all_languages()
            .iter()
            .map(|l| (l.name().to_string(), l.short_name().to_string()))
            .collect())
    }
}

use embedded_lang::LanguageSet;
use tauri::State;

use crate::managed_value::ManagedValue;

use super::markdown::MarkdownTree;

pub type ManagedTranslator = ManagedValue<LanguageSet>;

pub struct TranslatorManager {}
impl TranslatorManager {
    pub const UPDATED_EVENT: &'static str = "updated-language";

    pub fn read(state: &ManagedTranslator) -> Option<LanguageSet> {
        state.clone_inner()
    }

    pub fn translate(path: String, state: &ManagedTranslator) -> Option<String> {
        if let Some(translator) = Self::read(state) {
            translator.get(&path).and_then(|s| Some(s.to_string()))
        } else {
            None
        }
    }

    pub fn help_text(state: &ManagedTranslator) -> Option<MarkdownTree> {
        if let Some(translator) = Self::read(state) {
            translator.attachment("help")
        } else {
            None
        }
    }

    pub fn set_language(language: String, state: &ManagedTranslator) -> bool {
        if let Some(mut translator) = state.read() {
            translator.set_language(&language)
        } else {
            false
        }
    }

    pub fn list(state: &ManagedTranslator) -> Vec<(String, String)> {
        if let Some(translator) = Self::read(state) {
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

#[tauri::command]
pub fn translate(path: String, state: State<ManagedTranslator>) -> Option<String> {
    TranslatorManager::translate(path, state.inner())
}

#[tauri::command]
pub fn help_text(state: State<ManagedTranslator>) -> Option<MarkdownTree> {
    TranslatorManager::help_text(state.inner())
}

#[tauri::command]
pub fn list_languages(state: State<ManagedTranslator>) -> Vec<(String, String)> {
    TranslatorManager::list(state.inner())
}

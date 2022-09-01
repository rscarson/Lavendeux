use embedded_lang::{ LanguageSet, embedded_language };

pub const DEFAULT: &str = "en";

/// Create the language set
pub fn initialize() -> LanguageSet {
    LanguageSet::new(DEFAULT, &[
        embedded_language!("../../../language/en.lang.json"),
    ])
}
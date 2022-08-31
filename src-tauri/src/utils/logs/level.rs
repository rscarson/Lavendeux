/// Log levels
#[derive(Clone, serde::Serialize, serde::Deserialize)]
pub enum Level {
	Silly, Debug, Info, Warning, Error, Critical
}
impl Level {
    pub fn value(&self) -> i32 {
        match self {
            Self::Silly => -1,
            Self::Debug => 0,
            Self::Info => 1,
            Self::Warning => 2,
            Self::Error => 3,
            Self::Critical => 4,
        }
    }

    pub fn to_string(&self) -> String {
        match self {
            Self::Silly => "Silly".to_string(),
            Self::Debug => "Debug".to_string(),
            Self::Info => "Info".to_string(),
            Self::Warning => "Warning".to_string(),
            Self::Error => "Error".to_string(),
            Self::Critical => "Critical".to_string(),
        }
    }
}
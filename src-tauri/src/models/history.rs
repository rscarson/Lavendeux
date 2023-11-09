use core::fmt;
use serde::{Deserialize, Serialize};
use std::time::SystemTime;

#[derive(Deserialize, Serialize, Clone, Debug)]
pub enum SnippetResult {
    Success(String),
    Error(String),
}

impl fmt::Display for SnippetResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Success(e) => write!(f, "{}", e),
            Self::Error(e) => write!(f, "{}", e),
        }
    }
}

#[derive(Deserialize, Serialize, Clone, Debug)]
pub struct Snippet {
    pub input: String,
    pub result: SnippetResult,
    pub timestamp: SystemTime,
}

impl Default for Snippet {
    fn default() -> Self {
        Self {
            input: "".to_string(),
            result: SnippetResult::Error("".to_string()),
            timestamp: SystemTime::now(),
        }
    }
}

impl Snippet {
    pub fn new(input: &str, result: SnippetResult) -> Self {
        Self {
            input: input.to_string(),
            result,
            timestamp: SystemTime::now(),
        }
    }

    pub fn is_err(&self) -> bool {
        matches!(self.result, SnippetResult::Error(_))
    }
}

#[derive(Default, Deserialize, Serialize, Clone, Debug)]
pub struct History(Vec<Snippet>);
impl History {
    const MAX_LENGTH: usize = 50;

    pub fn clear(&mut self) {
        self.0.clear()
    }

    pub fn add(&mut self, item: Snippet) {
        self.0.insert(0, item);
        self.0.truncate(Self::MAX_LENGTH);
    }

    pub fn remove(&mut self, index: usize) -> bool {
        if index < self.0.len() {
            self.0.remove(index);
            true
        } else {
            false
        }
    }

    pub fn iter(&self) -> std::slice::Iter<'_, Snippet> {
        self.0.iter()
    }

    pub fn as_slice(&self) -> &[Snippet] {
        self.0.as_slice()
    }

    pub fn is_empty(&self) -> bool {
        self.0.is_empty()
    }
}

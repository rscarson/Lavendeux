use serde::{Deserialize, Serialize};

pub type DebugOutput = Vec<String>;

#[derive(PartialEq, Debug, Clone, Copy, Serialize, Deserialize)]
pub enum DebugType {
    Standard,
    FileSystem,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct DebugModel {
    pub output: DebugOutput,
    pub debug_type: DebugType,
}

impl Default for DebugModel {
    fn default() -> Self {
        Self {
            output: vec![],
            debug_type: DebugType::Standard,
        }
    }
}

impl DebugModel {
    const LOG_FILENAME: &'static str = "lavendeux.log";

    pub fn new() -> Self {
        Self::default()
    }

    pub fn debug(&mut self, s: String) {
        println!("{}", s);
        self.output.push(s.clone());
        if self.debug_type == DebugType::FileSystem {
            self.write_log(s);
        }
    }

    pub fn write_log(&self, s: String) {
        use std::fs::OpenOptions;
        use std::io::Write;

        let mut file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(Self::LOG_FILENAME)
            .expect("Could not open log file");

        if let Err(e) = writeln!(file, "{}", s) {
            println!("Could not write to log file: {:?}", e);
        }
    }
}

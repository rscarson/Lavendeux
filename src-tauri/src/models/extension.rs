use lavendeux_parser2::ExtensionDetails;
use std::collections::{HashMap, HashSet};

pub type Blacklist = HashSet<String>;
pub type Extensions = HashMap<String, Result<ExtensionDetails, String>>;

use lavendeux_parser::Extension;
use std::collections::{HashMap, HashSet};

pub type Blacklist = HashSet<String>;
pub type Extensions = HashMap<String, Result<Extension, String>>;

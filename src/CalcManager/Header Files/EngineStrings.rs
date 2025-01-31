use std::collections::HashMap;

lazy_static! {
    pub static ref ENGINE_STRINGS: HashMap<&'static str, &'static str> = {
        let mut m = HashMap::new();
        m.insert("SIDS_DECIMAL_SEPARATOR", ".");
        m.insert("SIDS_THOUSANDS_SEPARATOR", ",");
        m.insert("SIDS_GROUPING", "3;0");
        m.insert("SIDS_NUMBER", "0");
        m
    };
}

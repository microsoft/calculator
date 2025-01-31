pub trait IHistoryDisplay {
    fn add_item_to_history(&self, item: &str);
    fn remove_item_from_history(&self, index: usize);
    fn clear_history(&self);
    fn get_history_items(&self) -> Vec<String>;
}

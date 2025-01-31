pub trait ICalcDisplay {
    fn set_primary_display(&self, text: &str, is_error: bool);
    fn set_is_in_error(&self, is_in_error: bool);
    fn set_expression_display(
        &self,
        tokens: std::sync::Arc<std::vec::Vec<(String, i32)>>,
        commands: std::sync::Arc<std::vec::Vec<std::sync::Arc<dyn IExpressionCommand>>>,
    );
    fn set_parenthesis_number(&self, count: u32);
    fn on_no_right_paren_added(&self);
    fn max_digits_reached(&self);
    fn binary_operator_received(&self);
    fn on_history_item_added(&self, added_item_index: u32);
    fn set_memorized_numbers(&self, memorized_numbers: Vec<String>);
    fn memory_item_changed(&self, index_of_memory: u32);
    fn input_changed(&self);
}

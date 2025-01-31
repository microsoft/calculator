use std::collections::VecDeque;
use std::sync::Arc;

pub struct HistoryItemVector {
    pub tokens: Arc<Vec<(String, i32)>>,
    pub commands: Arc<Vec<Arc<dyn IExpressionCommand>>>,
    pub expression: String,
    pub result: String,
}

pub struct HistoryItem {
    pub history_item_vector: HistoryItemVector,
}

pub struct CalculatorHistory {
    max_history_size: usize,
    history_items: VecDeque<Arc<HistoryItem>>,
}

impl CalculatorHistory {
    pub fn new(max_size: usize) -> Self {
        Self {
            max_history_size: max_size,
            history_items: VecDeque::new(),
        }
    }

    pub fn add_to_history(
        &mut self,
        tokens: Arc<Vec<(String, i32)>>,
        commands: Arc<Vec<Arc<dyn IExpressionCommand>>>,
        result: &str,
    ) -> u32 {
        let history_item = Arc::new(HistoryItem {
            history_item_vector: HistoryItemVector {
                tokens: tokens.clone(),
                commands: commands.clone(),
                expression: Self::get_generated_expression(&tokens),
                result: result.to_string(),
            },
        });
        self.add_item(history_item)
    }

    pub fn add_item(&mut self, history_item: Arc<HistoryItem>) -> u32 {
        if self.history_items.len() >= self.max_history_size {
            self.history_items.pop_front();
        }
        self.history_items.push_back(history_item);
        (self.history_items.len() - 1) as u32
    }

    pub fn remove_item(&mut self, index: u32) -> bool {
        if (index as usize) < self.history_items.len() {
            self.history_items.remove(index as usize);
            true
        } else {
            false
        }
    }

    pub fn get_history(&self) -> &VecDeque<Arc<HistoryItem>> {
        &self.history_items
    }

    pub fn get_history_item(&self, index: u32) -> Option<&Arc<HistoryItem>> {
        self.history_items.get(index as usize)
    }

    pub fn clear_history(&mut self) {
        self.history_items.clear();
    }

    fn get_generated_expression(tokens: &[(String, i32)]) -> String {
        tokens
            .iter()
            .map(|(token, _)| token.clone())
            .collect::<Vec<_>>()
            .join(" ")
    }
}

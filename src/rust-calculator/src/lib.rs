use std::collections::VecDeque;

#[derive(Clone, Debug, PartialEq, Eq)]
pub enum CalculatorMode {
    Standard,
    Scientific,
    Programmer,
    Date,
    UnitConverter,
    Graphing,
}

#[derive(Clone, Debug, Default)]
pub struct CalculatorHistory {
    entries: VecDeque<String>,
    max_entries: usize,
}

impl CalculatorHistory {
    pub fn new(max_entries: usize) -> Self {
        Self {
            entries: VecDeque::new(),
            max_entries,
        }
    }

    pub fn push(&mut self, value: String) {
        if self.entries.len() >= self.max_entries {
            self.entries.pop_back();
        }
        self.entries.push_front(value);
    }

    pub fn clear(&mut self) {
        self.entries.clear();
    }

    pub fn entries(&self) -> impl Iterator<Item = &String> {
        self.entries.iter()
    }
}

#[derive(Clone, Debug)]
pub struct CalculatorEngine {
    mode: CalculatorMode,
    expression: String,
    memory: f64,
    history: CalculatorHistory,
}

impl Default for CalculatorEngine {
    fn default() -> Self {
        Self::new()
    }
}

impl CalculatorEngine {
    pub fn new() -> Self {
        Self {
            mode: CalculatorMode::Standard,
            expression: String::new(),
            memory: 0.0,
            history: CalculatorHistory::new(20),
        }
    }

    pub fn set_mode(&mut self, mode: CalculatorMode) {
        self.mode = mode;
    }

    pub fn mode(&self) -> &CalculatorMode {
        &self.mode
    }

    pub fn append_expression(&mut self, fragment: &str) {
        self.expression.push_str(fragment);
    }

    pub fn clear_expression(&mut self) {
        self.expression.clear();
    }

    pub fn expression(&self) -> &str {
        &self.expression
    }

    pub fn evaluate_expression(&mut self, input: &str) -> Result<f64, String> {
        let expression = if input.trim().is_empty() {
            self.expression.clone()
        } else {
            input.to_string()
        };

        let value = meval::eval_str(expression.as_str())
            .map_err(|err| format!("invalid expression: {err}"))?;

        self.history.push(format!("{expression} = {value}"));
        self.expression = value.to_string();
        Ok(value)
    }

    pub fn memory_add(&mut self, value: f64) {
        self.memory += value;
    }

    pub fn memory_subtract(&mut self, value: f64) {
        self.memory -= value;
    }

    pub fn memory_clear(&mut self) {
        self.memory = 0.0;
    }

    pub fn memory(&self) -> f64 {
        self.memory
    }

    pub fn history(&self) -> impl Iterator<Item = &String> {
        self.history.entries()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn evaluates_basic_math() {
        let mut engine = CalculatorEngine::new();
        let result = engine.evaluate_expression("2 + 3 * 4").unwrap();
        assert!((result - 14.0).abs() < f64::EPSILON);
    }

    #[test]
    fn memory_tracks_values() {
        let mut engine = CalculatorEngine::new();
        engine.memory_add(3.5);
        engine.memory_add(6.5);
        assert!((engine.memory() - 10.0).abs() < f64::EPSILON);
    }
}

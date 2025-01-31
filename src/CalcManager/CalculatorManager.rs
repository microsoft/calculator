use std::sync::Arc;
use std::collections::VecDeque;

use crate::calc_engine::CCalcEngine;
use crate::calc_history::CalculatorHistory;
use crate::calc_display::ICalcDisplay;
use crate::calc_input::CalcInput;
use crate::calc_utils::Rational;

pub enum CalculatorMode {
    Standard,
    Scientific,
}

pub enum CalculatorPrecision {
    StandardModePrecision = 16,
    ScientificModePrecision = 32,
    ProgrammerModePrecision = 64,
}

pub enum MemoryCommand {
    MemorizeNumber = 330,
    MemorizedNumberLoad = 331,
    MemorizedNumberAdd = 332,
    MemorizedNumberSubtract = 333,
    MemorizedNumberClearAll = 334,
    MemorizedNumberClear = 335,
}

pub struct CalculatorManager {
    display_callback: Arc<dyn ICalcDisplay>,
    current_calculator_engine: Option<Arc<CCalcEngine>>,
    scientific_calculator_engine: Option<Arc<CCalcEngine>>,
    standard_calculator_engine: Option<Arc<CCalcEngine>>,
    programmer_calculator_engine: Option<Arc<CCalcEngine>>,
    resource_provider: Arc<dyn IResourceProvider>,
    in_history_item_load_mode: bool,
    memorized_numbers: Vec<Rational>,
    persisted_primary_value: Rational,
    is_exponential_format: bool,
    current_degree_mode: Command,
    std_history: Arc<CalculatorHistory>,
    sci_history: Arc<CalculatorHistory>,
    history: Option<Arc<CalculatorHistory>>,
}

impl CalculatorManager {
    pub fn new(display_callback: Arc<dyn ICalcDisplay>, resource_provider: Arc<dyn IResourceProvider>) -> Self {
        let std_history = Arc::new(CalculatorHistory::new(20));
        let sci_history = Arc::new(CalculatorHistory::new(20));

        CCalcEngine::initial_one_time_only_setup(resource_provider.clone());

        Self {
            display_callback,
            current_calculator_engine: None,
            scientific_calculator_engine: None,
            standard_calculator_engine: None,
            programmer_calculator_engine: None,
            resource_provider,
            in_history_item_load_mode: false,
            memorized_numbers: Vec::new(),
            persisted_primary_value: Rational::default(),
            is_exponential_format: false,
            current_degree_mode: Command::CommandNULL,
            std_history: std_history.clone(),
            sci_history: sci_history.clone(),
            history: None,
        }
    }

    pub fn set_primary_display(&self, display_string: &str, is_error: bool) {
        if !self.in_history_item_load_mode {
            self.display_callback.set_primary_display(display_string, is_error);
        }
    }

    pub fn set_is_in_error(&self, is_error: bool) {
        self.display_callback.set_is_in_error(is_error);
    }

    pub fn display_paste_error(&self) {
        if let Some(engine) = &self.current_calculator_engine {
            engine.display_error(CALC_E_DOMAIN);
        }
    }

    pub fn max_digits_reached(&self) {
        self.display_callback.max_digits_reached();
    }

    pub fn binary_operator_received(&self) {
        self.display_callback.binary_operator_received();
    }

    pub fn memory_item_changed(&self, index_of_memory: u32) {
        self.display_callback.memory_item_changed(index_of_memory);
    }

    pub fn input_changed(&self) {
        self.display_callback.input_changed();
    }

    pub fn set_expression_display(
        &self,
        tokens: Arc<Vec<(String, i32)>>,
        commands: Arc<Vec<Arc<dyn IExpressionCommand>>>,
    ) {
        if !self.in_history_item_load_mode {
            self.display_callback.set_expression_display(tokens, commands);
        }
    }

    pub fn set_memorized_numbers(&self, memorized_numbers: Vec<String>) {
        self.display_callback.set_memorized_numbers(memorized_numbers);
    }

    pub fn set_parenthesis_number(&self, parenthesis_count: u32) {
        self.display_callback.set_parenthesis_number(parenthesis_count);
    }

    pub fn on_no_right_paren_added(&self) {
        self.display_callback.on_no_right_paren_added();
    }

    pub fn reset(&mut self, clear_memory: bool) {
        self.set_standard_mode();

        if let Some(engine) = &self.scientific_calculator_engine {
            engine.process_command(IDC_DEG);
            engine.process_command(IDC_CLEAR);

            if self.is_exponential_format {
                self.is_exponential_format = false;
                engine.process_command(IDC_FE);
            }
        }

        if let Some(engine) = &self.programmer_calculator_engine {
            engine.process_command(IDC_CLEAR);
        }

        if clear_memory {
            self.memorized_number_clear_all();
        }
    }

    pub fn set_standard_mode(&mut self) {
        if self.standard_calculator_engine.is_none() {
            self.standard_calculator_engine = Some(Arc::new(CCalcEngine::new(
                false,
                false,
                self.resource_provider.clone(),
                self.display_callback.clone(),
                self.std_history.clone(),
            )));
        }

        self.current_calculator_engine = self.standard_calculator_engine.clone();
        if let Some(engine) = &self.current_calculator_engine {
            engine.process_command(IDC_DEC);
            engine.process_command(IDC_CLEAR);
            engine.change_precision(CalculatorPrecision::StandardModePrecision as i32);
            self.update_max_int_digits();
        }
        self.history = Some(self.std_history.clone());
    }

    pub fn set_scientific_mode(&mut self) {
        if self.scientific_calculator_engine.is_none() {
            self.scientific_calculator_engine = Some(Arc::new(CCalcEngine::new(
                true,
                false,
                self.resource_provider.clone(),
                self.display_callback.clone(),
                self.sci_history.clone(),
            )));
        }

        self.current_calculator_engine = self.scientific_calculator_engine.clone();
        if let Some(engine) = &self.current_calculator_engine {
            engine.process_command(IDC_DEC);
            engine.process_command(IDC_CLEAR);
            engine.change_precision(CalculatorPrecision::ScientificModePrecision as i32);
        }
        self.history = Some(self.sci_history.clone());
    }

    pub fn set_programmer_mode(&mut self) {
        if self.programmer_calculator_engine.is_none() {
            self.programmer_calculator_engine = Some(Arc::new(CCalcEngine::new(
                true,
                true,
                self.resource_provider.clone(),
                self.display_callback.clone(),
                None,
            )));
        }

        self.current_calculator_engine = self.programmer_calculator_engine.clone();
        if let Some(engine) = &self.current_calculator_engine {
            engine.process_command(IDC_DEC);
            engine.process_command(IDC_CLEAR);
            engine.change_precision(CalculatorPrecision::ProgrammerModePrecision as i32);
        }
    }

    pub fn send_command(&mut self, command: Command) {
        if command == Command::CommandCLEAR
            || command == Command::CommandEQU
            || command == Command::ModeBasic
            || command == Command::ModeScientific
            || command == Command::ModeProgrammer
        {
            match command {
                Command::ModeBasic => self.set_standard_mode(),
                Command::ModeScientific => self.set_scientific_mode(),
                Command::ModeProgrammer => self.set_programmer_mode(),
                _ => {
                    if let Some(engine) = &self.current_calculator_engine {
                        engine.process_command(command as i32);
                    }
                }
            }

            self.input_changed();
            return;
        }

        if command == Command::CommandDEG
            || command == Command::CommandRAD
            || command == Command::CommandGRAD
        {
            self.current_degree_mode = command;
        }

        if let Some(engine) = &self.current_calculator_engine {
            match command {
                Command::CommandASIN => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandSIN as i32);
                }
                Command::CommandACOS => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandCOS as i32);
                }
                Command::CommandATAN => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandTAN as i32);
                }
                Command::CommandPOWE => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandLN as i32);
                }
                Command::CommandASINH => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandSINH as i32);
                }
                Command::CommandACOSH => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandCOSH as i32);
                }
                Command::CommandATANH => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandTANH as i32);
                }
                Command::CommandASEC => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandSEC as i32);
                }
                Command::CommandACSC => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandCSC as i32);
                }
                Command::CommandACOT => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandCOT as i32);
                }
                Command::CommandASECH => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandSECH as i32);
                }
                Command::CommandACSCH => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandCSCH as i32);
                }
                Command::CommandACOTH => {
                    engine.process_command(Command::CommandINV as i32);
                    engine.process_command(Command::CommandCOTH as i32);
                }
                Command::CommandFE => {
                    self.is_exponential_format = !self.is_exponential_format;
                    engine.process_command(command as i32);
                }
                _ => {
                    engine.process_command(command as i32);
                }
            }
        }

        self.input_changed();
    }

    pub fn load_persisted_primary_value(&mut self) {
        if let Some(engine) = &self.current_calculator_engine {
            engine.persisted_mem_object(self.persisted_primary_value.clone());
            engine.process_command(IDC_RECALL);
        }
        self.input_changed();
    }

    pub fn memorize_number(&mut self) {
        if let Some(engine) = &self.current_calculator_engine {
            if engine.f_in_error_state() {
                return;
            }

            engine.process_command(IDC_STORE);

            if let Some(memory_object) = engine.persisted_mem_object() {
                self.memorized_numbers.insert(0, memory_object);
            }

            if self.memorized_numbers.len() > 100 {
                self.memorized_numbers.truncate(100);
            }

            self.set_memorized_numbers_string();
        }
    }

    pub fn memorized_number_load(&mut self, index_of_memory: u32) {
        if let Some(engine) = &self.current_calculator_engine {
            if engine.f_in_error_state() {
                return;
            }

            self.memorized_number_select(index_of_memory);
            engine.process_command(IDC_RECALL);
        }
        self.input_changed();
    }

    pub fn memorized_number_add(&mut self, index_of_memory: u32) {
        if let Some(engine) = &self.current_calculator_engine {
            if engine.f_in_error_state() {
                return;
            }

            if self.memorized_numbers.is_empty() {
                self.memorize_number();
            } else {
                self.memorized_number_select(index_of_memory);
                engine.process_command(IDC_MPLUS);

                self.memorized_number_changed(index_of_memory);
                self.set_memorized_numbers_string();
            }

            self.display_callback.memory_item_changed(index_of_memory);
        }
    }

    pub fn memorized_number_clear(&mut self, index_of_memory: u32) {
        if index_of_memory < self.memorized_numbers.len() as u32 {
            self.memorized_numbers.remove(index_of_memory as usize);
        }
    }

    pub fn memorized_number_subtract(&mut self, index_of_memory: u32) {
        if let Some(engine) = &self.current_calculator_engine {
            if engine.f_in_error_state() {
                return;
            }

            if self.memorized_numbers.is_empty() {
                self.memorize_number();
                self.memorized_number_subtract(0);
                self.memorized_number_subtract(0);
            } else {
                self.memorized_number_select(index_of_memory);
                engine.process_command(IDC_MMINUS);

                self.memorized_number_changed(index_of_memory);
                self.set_memorized_numbers_string();
            }

            self.display_callback.memory_item_changed(index_of_memory);
        }
    }

    pub fn memorized_number_clear_all(&mut self) {
        self.memorized_numbers.clear();

        if let Some(engine) = &self.current_calculator_engine {
            engine.process_command(IDC_MCLEAR);
        }
        self.set_memorized_numbers_string();
    }

    fn memorized_number_select(&mut self, index_of_memory: u32) {
        if let Some(engine) = &self.current_calculator_engine {
            if engine.f_in_error_state() {
                return;
            }

            if let Some(memory_object) = self.memorized_numbers.get(index_of_memory as usize) {
                engine.persisted_mem_object(memory_object.clone());
            }
        }
    }

    fn memorized_number_changed(&mut self, index_of_memory: u32) {
        if let Some(engine) = &self.current_calculator_engine {
            if engine.f_in_error_state() {
                return;
            }

            if let Some(memory_object) = engine.persisted_mem_object() {
                if let Some(mem) = self.memorized_numbers.get_mut(index_of_memory as usize) {
                    *mem = memory_object;
                }
            }
        }
    }

    pub fn get_history_items(&self) -> Vec<Arc<HISTORYITEM>> {
        if let Some(history) = &self.history {
            history.get_history().clone()
        } else {
            Vec::new()
        }
    }

    pub fn get_history_items_by_mode(&self, mode: CalculatorMode) -> Vec<Arc<HISTORYITEM>> {
        match mode {
            CalculatorMode::Standard => self.std_history.get_history().clone(),
            CalculatorMode::Scientific => self.sci_history.get_history().clone(),
        }
    }

    pub fn set_history_items(&mut self, history_items: Vec<Arc<HISTORYITEM>>) {
        if let Some(history) = &self.history {
            for item in history_items {
                let index = history.add_item(item);
                self.on_history_item_added(index);
            }
        }
    }

    pub fn get_history_item(&self, index: u32) -> Option<Arc<HISTORYITEM>> {
        if let Some(history) = &self.history {
            history.get_history_item(index)
        } else {
            None
        }
    }

    pub fn on_history_item_added(&self, added_item_index: u32) {
        self.display_callback.on_history_item_added(added_item_index);
    }

    pub fn remove_history_item(&mut self, index: u32) -> bool {
        if let Some(history) = &self.history {
            history.remove_item(index)
        } else {
            false
        }
    }

    pub fn clear_history(&mut self) {
        if let Some(history) = &self.history {
            history.clear_history();
        }
    }

    pub fn set_radix(&mut self, radix_type: RadixType) {
        if let Some(engine) = &self.current_calculator_engine {
            match radix_type {
                RadixType::Hex => engine.process_command(IDC_HEX),
                RadixType::Decimal => engine.process_command(IDC_DEC),
                RadixType::Octal => engine.process_command(IDC_OCT),
                RadixType::Binary => engine.process_command(IDC_BIN),
            }
        }
        self.set_memorized_numbers_string();
    }

    fn set_memorized_numbers_string(&self) {
        if let Some(engine) = &self.current_calculator_engine {
            let mut result_vector = Vec::new();
            for memory_item in &self.memorized_numbers {
                let radix = engine.get_current_radix();
                let string_value = engine.get_string_for_display(memory_item, radix);

                if !string_value.is_empty() {
                    result_vector.push(engine.group_digits_per_radix(&string_value, radix));
                }
            }
            self.display_callback.set_memorized_numbers(result_vector);
        }
    }

    pub fn get_current_degree_mode(&self) -> Command {
        if self.current_degree_mode == Command::CommandNULL {
            self.current_degree_mode = Command::CommandDEG;
        }
        self.current_degree_mode
    }

    pub fn get_result_for_radix(&self, radix: u32, precision: i32, group_digits_per_radix: bool) -> String {
        if let Some(engine) = &self.current_calculator_engine {
            engine.get_current_result_for_radix(radix, precision, group_digits_per_radix)
        } else {
            String::new()
        }
    }

    pub fn set_precision(&mut self, precision: i32) {
        if let Some(engine) = &self.current_calculator_engine {
            engine.change_precision(precision);
        }
    }

    pub fn update_max_int_digits(&mut self) {
        if let Some(engine) = &self.current_calculator_engine {
            engine.update_max_int_digits();
        }
    }

    pub fn decimal_separator(&self) -> char {
        if let Some(engine) = &self.current_calculator_engine {
            engine.decimal_separator()
        } else {
            self.resource_provider.get_cengine_string("sDecimal").chars().next().unwrap_or('.')
        }
    }

    pub fn is_engine_recording(&self) -> bool {
        if let Some(engine) = &self.current_calculator_engine {
            engine.f_in_recording_state()
        } else {
            false
        }
    }

    pub fn is_input_empty(&self) -> bool {
        if let Some(engine) = &self.current_calculator_engine {
            engine.is_input_empty()
        } else {
            true
        }
    }

    pub fn set_in_history_item_load_mode(&mut self, is_history_item_load_mode: bool) {
        self.in_history_item_load_mode = is_history_item_load_mode;
    }

    pub fn get_display_commands_snapshot(&self) -> Vec<Arc<dyn IExpressionCommand>> {
        if let Some(engine) = &self.current_calculator_engine {
            engine.get_history_collector_commands_snapshot()
        } else {
            Vec::new()
        }
    }
}

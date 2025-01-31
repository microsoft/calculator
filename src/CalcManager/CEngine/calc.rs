use std::collections::HashMap;
use std::sync::Arc;

use crate::calc_input::CalcInput;
use crate::calc_utils::{Rational, RationalMath};
use crate::calc_display::ICalcDisplay;
use crate::calc_history::CHistoryCollector;
use crate::calc_resource::IResourceProvider;

const DEFAULT_MAX_DIGITS: i32 = 32;
const DEFAULT_PRECISION: i32 = 32;
const DEFAULT_RADIX: i32 = 10;

const DEFAULT_DEC_SEPARATOR: char = '.';
const DEFAULT_GRP_SEPARATOR: char = ',';
const DEFAULT_GRP_STR: &str = "3;0";
const DEFAULT_NUMBER_STR: &str = "0";

pub struct CCalcEngine {
    f_precedence: bool,
    f_integer_mode: bool,
    p_calc_display: Option<Arc<dyn ICalcDisplay>>,
    resource_provider: Arc<dyn IResourceProvider>,
    n_op_code: i32,
    n_prev_op_code: i32,
    b_change_op: bool,
    b_record: bool,
    b_set_calc_state: bool,
    input: CalcInput,
    n_fe: NumberFormat,
    memory_value: Rational,
    hold_val: Rational,
    current_val: Rational,
    last_val: Rational,
    paren_vals: Vec<Rational>,
    precedence_vals: Vec<Rational>,
    b_error: bool,
    b_inv: bool,
    b_no_prev_equ: bool,
    radix: i32,
    precision: i32,
    c_int_digits_sav: i32,
    dec_grouping: Vec<i32>,
    number_string: String,
    n_temp_com: i32,
    open_paren_count: usize,
    n_op: Vec<i32>,
    n_prec_op: Vec<i32>,
    precedence_op_count: usize,
    n_last_com: i32,
    angletype: AngleType,
    numwidth: NUM_WIDTH,
    dw_word_bit_width: i32,
    history_collector: CHistoryCollector,
    group_separator: char,
    chop_numbers: Vec<Rational>,
    max_decimal_value_strings: Vec<String>,
    decimal_separator: char,
}

impl CCalcEngine {
    pub fn new(
        f_precedence: bool,
        f_integer_mode: bool,
        resource_provider: Arc<dyn IResourceProvider>,
        p_calc_display: Option<Arc<dyn ICalcDisplay>>,
        p_history_display: Option<Arc<dyn IHistoryDisplay>>,
    ) -> Self {
        let mut engine = CCalcEngine {
            f_precedence,
            f_integer_mode,
            p_calc_display: p_calc_display.clone(),
            resource_provider: resource_provider.clone(),
            n_op_code: 0,
            n_prev_op_code: 0,
            b_change_op: false,
            b_record: false,
            b_set_calc_state: false,
            input: CalcInput::new(DEFAULT_DEC_SEPARATOR),
            n_fe: NumberFormat::Float,
            memory_value: Rational::default(),
            hold_val: Rational::default(),
            current_val: Rational::default(),
            last_val: Rational::default(),
            paren_vals: vec![Rational::default(); MAXPRECDEPTH],
            precedence_vals: vec![Rational::default(); MAXPRECDEPTH],
            b_error: false,
            b_inv: false,
            b_no_prev_equ: true,
            radix: DEFAULT_RADIX,
            precision: DEFAULT_PRECISION,
            c_int_digits_sav: DEFAULT_MAX_DIGITS,
            dec_grouping: vec![],
            number_string: DEFAULT_NUMBER_STR.to_string(),
            n_temp_com: 0,
            open_paren_count: 0,
            n_op: vec![0; MAXPRECDEPTH],
            n_prec_op: vec![0; MAXPRECDEPTH],
            precedence_op_count: 0,
            n_last_com: 0,
            angletype: AngleType::Degrees,
            numwidth: NUM_WIDTH::QWORD_WIDTH,
            dw_word_bit_width: 0,
            history_collector: CHistoryCollector::new(p_calc_display, p_history_display, DEFAULT_DEC_SEPARATOR),
            group_separator: DEFAULT_GRP_SEPARATOR,
            chop_numbers: vec![Rational::default(); NUM_WIDTH_LENGTH],
            max_decimal_value_strings: vec![String::new(); NUM_WIDTH_LENGTH],
            decimal_separator: DEFAULT_DEC_SEPARATOR,
        };

        engine.init_chop_numbers();
        engine.dw_word_bit_width = engine.dw_word_bit_width_from_num_width(engine.numwidth);
        engine.max_trigonometric_num = RationalMath::pow(10, 100);
        engine.set_radix_type_and_num_width(RadixType::Decimal, engine.numwidth);
        engine.settings_changed();
        engine.display_num();

        engine
    }

    fn init_chop_numbers(&mut self) {
        self.chop_numbers[0] = Rational::from_rat(rat_qword);
        self.chop_numbers[1] = Rational::from_rat(rat_dword);
        self.chop_numbers[2] = Rational::from_rat(rat_word);
        self.chop_numbers[3] = Rational::from_rat(rat_byte);

        for i in 0..self.chop_numbers.len() {
            let max_val = self.chop_numbers[i] / 2;
            let max_val = RationalMath::integer(max_val);
            self.max_decimal_value_strings[i] = max_val.to_string(10, NumberFormat::Float, self.precision);
        }
    }

    fn get_chop_number(&self) -> Rational {
        self.chop_numbers[self.numwidth as usize].clone()
    }

    fn get_max_decimal_value_string(&self) -> String {
        self.max_decimal_value_strings[self.numwidth as usize].clone()
    }

    pub fn persisted_mem_object(&self) -> Rational {
        self.memory_value.clone()
    }

    pub fn set_persisted_mem_object(&mut self, mem_object: Rational) {
        self.memory_value = mem_object;
    }

    pub fn settings_changed(&mut self) {
        let last_dec = self.decimal_separator;
        let dec_str = self.resource_provider.get_cengine_string("sDecimal");
        self.decimal_separator = if dec_str.is_empty() {
            DEFAULT_DEC_SEPARATOR
        } else {
            dec_str.chars().next().unwrap()
        };

        let last_sep = self.group_separator;
        let sep_str = self.resource_provider.get_cengine_string("sThousand");
        self.group_separator = if sep_str.is_empty() {
            DEFAULT_GRP_SEPARATOR
        } else {
            sep_str.chars().next().unwrap()
        };

        let last_dec_grouping = self.dec_grouping.clone();
        let grp_str = self.resource_provider.get_cengine_string("sGrouping");
        self.dec_grouping = if grp_str.is_empty() {
            digit_grouping_string_to_grouping_vector(DEFAULT_GRP_STR)
        } else {
            digit_grouping_string_to_grouping_vector(&grp_str)
        };

        let mut num_changed = false;

        if self.dec_grouping != last_dec_grouping || self.group_separator != last_sep {
            num_changed = true;
        }

        if self.decimal_separator != last_dec {
            self.input.set_decimal_symbol(self.decimal_separator);
            self.history_collector.set_decimal_symbol(self.decimal_separator);
            s_engine_strings.insert(SIDS_DECIMAL_SEPARATOR.to_string(), self.decimal_separator.to_string());
            num_changed = true;
        }

        if num_changed {
            self.display_num();
        }
    }

    pub fn decimal_separator(&self) -> char {
        self.decimal_separator
    }

    pub fn get_history_collector_commands_snapshot(&self) -> Vec<Arc<dyn IExpressionCommand>> {
        let mut commands = self.history_collector.get_commands();
        if !self.history_collector.f_opnd_added_to_history() && self.b_record {
            commands.push(self.history_collector.get_operand_commands_from_string(&self.number_string, &self.current_val));
        }
        commands
    }

    pub fn initial_one_time_only_setup(resource_provider: Arc<dyn IResourceProvider>) {
        Self::load_engine_strings(resource_provider);
        Self::change_base_constants(DEFAULT_RADIX, DEFAULT_MAX_DIGITS, DEFAULT_PRECISION);
    }

    fn load_engine_strings(resource_provider: Arc<dyn IResourceProvider>) {
        for sid in G_SIDS.iter() {
            let loc_string = resource_provider.get_cengine_string(sid);
            if !loc_string.is_empty() {
                s_engine_strings.insert(sid.to_string(), loc_string);
            }
        }
    }

    fn change_base_constants(radix: i32, max_int_digits: i32, precision: i32) {
        // Implementation of ChangeBaseConstants
    }

    fn display_num(&self) {
        // Implementation of DisplayNum
    }

    fn set_radix_type_and_num_width(&mut self, radix_type: RadixType, num_width: NUM_WIDTH) {
        // Implementation of SetRadixTypeAndNumWidth
    }

    fn dw_word_bit_width_from_num_width(&self, num_width: NUM_WIDTH) -> i32 {
        // Implementation of DwWordBitWidthFromNumWidth
    }
}

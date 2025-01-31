use std::sync::{Arc, Mutex};
use std::collections::VecDeque;

use crate::calc_engine::CCalcEngine;
use crate::calc_display::ICalcDisplay;
use crate::calc_history::IHistoryDisplay;
use crate::calc_utils::Rational;

const MAXPRECDEPTH: usize = 25;

pub struct CHistoryCollector {
    p_history_display: Option<Arc<dyn IHistoryDisplay>>,
    p_calc_display: Option<Arc<dyn ICalcDisplay>>,
    i_cur_line_hist_start: i32,
    last_op_start_index: i32,
    last_bin_op_start_index: i32,
    operand_indices: [i32; MAXPRECDEPTH],
    cur_operand_index: i32,
    b_last_opnd_brace: bool,
    decimal_symbol: char,
    sp_tokens: Option<Arc<Mutex<VecDeque<(String, i32)>>>>,
    sp_commands: Option<Arc<Mutex<VecDeque<Arc<dyn IExpressionCommand>>>>>,
}

impl CHistoryCollector {
    pub fn new(
        p_calc_display: Option<Arc<dyn ICalcDisplay>>,
        p_history_display: Option<Arc<dyn IHistoryDisplay>>,
        decimal_symbol: char,
    ) -> Self {
        Self {
            p_history_display,
            p_calc_display,
            i_cur_line_hist_start: -1,
            last_op_start_index: -1,
            last_bin_op_start_index: -1,
            operand_indices: [-1; MAXPRECDEPTH],
            cur_operand_index: 0,
            b_last_opnd_brace: false,
            decimal_symbol,
            sp_tokens: None,
            sp_commands: None,
        }
    }

    pub fn add_opnd_to_history(&mut self, num_str: &str, rat: &Rational, f_repetition: bool) {
        let i_command_end = self.add_command(self.get_operand_commands_from_string(num_str, rat));
        self.last_op_start_index = self.ich_add_sz_to_equation_sz(num_str, i_command_end);

        if f_repetition {
            self.set_expression_display();
        }
        self.b_last_opnd_brace = false;
        self.last_bin_op_start_index = -1;
    }

    pub fn remove_last_opnd_from_history(&mut self) {
        self.truncate_equation_sz_from_ich(self.last_op_start_index);
        self.set_expression_display();
        self.last_op_start_index = -1;
    }

    pub fn add_bin_op_to_history(&mut self, n_op_code: i32, is_integer_mode: bool, f_no_repetition: bool) {
        let i_command_end = self.add_command(Arc::new(CBinaryCommand::new(n_op_code)));
        self.last_bin_op_start_index = self.ich_add_sz_to_equation_sz(" ", -1);

        self.ich_add_sz_to_equation_sz(&CCalcEngine::op_code_to_binary_string(n_op_code, is_integer_mode), i_command_end);
        self.ich_add_sz_to_equation_sz(" ", -1);

        if f_no_repetition {
            self.set_expression_display();
        }
        self.last_op_start_index = -1;
    }

    pub fn change_last_bin_op(&mut self, n_op_code: i32, f_prec_inv_to_higher: bool, is_integer_mode: bool) {
        self.truncate_equation_sz_from_ich(self.last_bin_op_start_index);
        if f_prec_inv_to_higher {
            self.enclose_prec_inversion_brackets();
        }
        self.add_bin_op_to_history(n_op_code, is_integer_mode, true);
    }

    pub fn push_last_opnd_start(&mut self, ich_opnd_start: i32) {
        let ich = if ich_opnd_start == -1 {
            self.last_op_start_index
        } else {
            ich_opnd_start
        };

        if self.cur_operand_index < self.operand_indices.len() as i32 {
            self.operand_indices[self.cur_operand_index as usize] = ich;
            self.cur_operand_index += 1;
        }
    }

    pub fn pop_last_opnd_start(&mut self) {
        if self.cur_operand_index > 0 {
            self.cur_operand_index -= 1;
            self.last_op_start_index = self.operand_indices[self.cur_operand_index as usize];
        }
    }

    pub fn add_open_brace_to_history(&mut self) {
        self.add_command(Arc::new(CParentheses::new(IDC_OPENP)));
        let ich_opnd_start = self.ich_add_sz_to_equation_sz(&CCalcEngine::op_code_to_string(IDC_OPENP), -1);
        self.push_last_opnd_start(ich_opnd_start);

        self.set_expression_display();
        self.last_bin_op_start_index = -1;
    }

    pub fn add_close_brace_to_history(&mut self) {
        self.add_command(Arc::new(CParentheses::new(IDC_CLOSEP)));
        self.ich_add_sz_to_equation_sz(&CCalcEngine::op_code_to_string(IDC_CLOSEP), -1);
        self.set_expression_display();
        self.pop_last_opnd_start();

        self.last_bin_op_start_index = -1;
        self.b_last_opnd_brace = true;
    }

    pub fn enclose_prec_inversion_brackets(&mut self) {
        let ich_start = if self.cur_operand_index > 0 {
            self.operand_indices[self.cur_operand_index as usize - 1]
        } else {
            0
        };

        self.insert_sz_in_equation_sz(&CCalcEngine::op_code_to_string(IDC_OPENP), -1, ich_start);
        self.ich_add_sz_to_equation_sz(&CCalcEngine::op_code_to_string(IDC_CLOSEP), -1);
    }

    pub fn f_opnd_added_to_history(&self) -> bool {
        self.last_op_start_index != -1
    }

    pub fn complete_history_line(&mut self, num_str: &str) {
        if let Some(p_history_display) = &self.p_history_display {
            let added_item_index = p_history_display.add_to_history(
                self.sp_tokens.clone().unwrap(),
                self.sp_commands.clone().unwrap(),
                num_str,
            );
            if let Some(p_calc_display) = &self.p_calc_display {
                p_calc_display.on_history_item_added(added_item_index);
            }
        }

        self.sp_tokens = None;
        self.sp_commands = None;
        self.i_cur_line_hist_start = -1;
        self.reinit_history();
    }

    pub fn complete_equation(&mut self, num_str: &str) {
        self.ich_add_sz_to_equation_sz(&CCalcEngine::op_code_to_string(IDC_EQU), -1);
        self.set_expression_display();
        self.complete_history_line(num_str);
    }

    pub fn clear_history_line(&mut self, err_str: &str) {
        if err_str.is_empty() {
            if let Some(p_calc_display) = &self.p_calc_display {
                p_calc_display.set_expression_display(
                    Arc::new(Mutex::new(VecDeque::new())),
                    Arc::new(Mutex::new(VecDeque::new())),
                );
            }
            self.i_cur_line_hist_start = -1;
            self.reinit_history();
        }
    }

    fn ich_add_sz_to_equation_sz(&mut self, str: &str, i_command_index: i32) -> i32 {
        if self.sp_tokens.is_none() {
            self.sp_tokens = Some(Arc::new(Mutex::new(VecDeque::new())));
        }

        let mut sp_tokens = self.sp_tokens.as_ref().unwrap().lock().unwrap();
        sp_tokens.push_back((str.to_string(), i_command_index));
        (sp_tokens.len() - 1) as i32
    }

    fn insert_sz_in_equation_sz(&mut self, str: &str, i_command_index: i32, ich: i32) {
        let mut sp_tokens = self.sp_tokens.as_ref().unwrap().lock().unwrap();
        sp_tokens.insert(ich as usize, (str.to_string(), i_command_index));
    }

    fn truncate_equation_sz_from_ich(&mut self, ich: i32) {
        let mut sp_tokens = self.sp_tokens.as_ref().unwrap().lock().unwrap();
        let mut sp_commands = self.sp_commands.as_ref().unwrap().lock().unwrap();

        let mut min_idx = -1;
        let n_tokens = sp_tokens.len();

        for i in ich as usize..n_tokens {
            let cur_token_id = sp_tokens[i].1;
            if cur_token_id != -1 {
                if min_idx == -1 || cur_token_id < min_idx {
                    min_idx = cur_token_id;
                    sp_commands.truncate(min_idx as usize);
                }
            }
        }

        sp_tokens.truncate(ich as usize);
    }

    fn set_expression_display(&self) {
        if let Some(p_calc_display) = &self.p_calc_display {
            p_calc_display.set_expression_display(
                self.sp_tokens.clone().unwrap(),
                self.sp_commands.clone().unwrap(),
            );
        }
    }

    fn add_command(&mut self, sp_command: Arc<dyn IExpressionCommand>) -> i32 {
        if self.sp_commands.is_none() {
            self.sp_commands = Some(Arc::new(Mutex::new(VecDeque::new())));
        }

        let mut sp_commands = self.sp_commands.as_ref().unwrap().lock().unwrap();
        sp_commands.push_back(sp_command);
        (sp_commands.len() - 1) as i32
    }

    pub fn update_history_expression(&mut self, radix: u32, precision: i32) {
        if self.sp_tokens.is_none() {
            return;
        }

        let mut sp_tokens = self.sp_tokens.as_ref().unwrap().lock().unwrap();
        let sp_commands = self.sp_commands.as_ref().unwrap().lock().unwrap();

        for token in sp_tokens.iter_mut() {
            let command_position = token.1;
            if command_position != -1 {
                let exp_command = &sp_commands[command_position as usize];
                if exp_command.get_command_type() == CommandType::OperandCommand {
                    let opnd_command = exp_command.as_any().downcast_ref::<COpndCommand>().unwrap();
                    token.0 = opnd_command.get_string(radix, precision);
                    opnd_command.set_commands(self.get_operand_commands_from_string(&token.0));
                }
            }
        }

        self.set_expression_display();
    }

    pub fn set_decimal_symbol(&mut self, decimal_symbol: char) {
        self.decimal_symbol = decimal_symbol;
    }

    fn get_operand_commands_from_string(&self, num_str: &str) -> Arc<dyn IExpressionCommand> {
        let mut commands = Vec::new();
        let f_negative = num_str.starts_with('-');

        for ch in num_str.chars().skip(if f_negative { 1 } else { 0 }) {
            match ch {
                ch if ch == self.decimal_symbol => commands.push(IDC_PNT),
                'e' => commands.push(IDC_EXP),
                '-' => commands.push(IDC_SIGN),
                '+' => {}
                ch => {
                    let num = ch as i32 - '0' as i32;
                    commands.push(num + IDC_0);
                }
            }
        }

        if f_negative {
            commands.push(IDC_SIGN);
        }

        Arc::new(COpndCommand::new(commands, f_negative, num_str.contains(self.decimal_symbol), num_str.contains('e')))
    }

    fn reinit_history(&mut self) {
        self.last_op_start_index = -1;
        self.last_bin_op_start_index = -1;
        self.cur_operand_index = 0;
        self.b_last_opnd_brace = false;
        if let Some(sp_tokens) = &self.sp_tokens {
            sp_tokens.lock().unwrap().clear();
        }
        if let Some(sp_commands) = &self.sp_commands {
            sp_commands.lock().unwrap().clear();
        }
    }
}

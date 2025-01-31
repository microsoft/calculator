use std::sync::Arc;

use crate::calc_engine::CCalcEngine;
use crate::calc_utils::Rational;
use crate::calc_display::ICalcDisplay;
use crate::calc_history::CHistoryCollector;
use crate::calc_resource::IResourceProvider;

pub struct CParentheses {
    command: i32,
}

impl CParentheses {
    pub fn new(command: i32) -> Self {
        Self { command }
    }

    pub fn get_command(&self) -> i32 {
        self.command
    }

    pub fn get_command_type(&self) -> CommandType {
        CommandType::Parentheses
    }

    pub fn accept(&self, command_visitor: &mut dyn ISerializeCommandVisitor) {
        command_visitor.visit_parentheses(self);
    }
}

pub struct CUnaryCommand {
    commands: Arc<Vec<i32>>,
}

impl CUnaryCommand {
    pub fn new(command: i32) -> Self {
        Self {
            commands: Arc::new(vec![command]),
        }
    }

    pub fn new_with_two_commands(command1: i32, command2: i32) -> Self {
        Self {
            commands: Arc::new(vec![command1, command2]),
        }
    }

    pub fn get_commands(&self) -> Arc<Vec<i32>> {
        Arc::clone(&self.commands)
    }

    pub fn get_command_type(&self) -> CommandType {
        CommandType::UnaryCommand
    }

    pub fn set_command(&mut self, command: i32) {
        Arc::make_mut(&mut self.commands).clear();
        Arc::make_mut(&mut self.commands).push(command);
    }

    pub fn set_commands(&mut self, command1: i32, command2: i32) {
        Arc::make_mut(&mut self.commands).clear();
        Arc::make_mut(&mut self.commands).push(command1);
        Arc::make_mut(&mut self.commands).push(command2);
    }

    pub fn accept(&self, command_visitor: &mut dyn ISerializeCommandVisitor) {
        command_visitor.visit_unary_command(self);
    }
}

pub struct CBinaryCommand {
    command: i32,
}

impl CBinaryCommand {
    pub fn new(command: i32) -> Self {
        Self { command }
    }

    pub fn set_command(&mut self, command: i32) {
        self.command = command;
    }

    pub fn get_command(&self) -> i32 {
        self.command
    }

    pub fn get_command_type(&self) -> CommandType {
        CommandType::BinaryCommand
    }

    pub fn accept(&self, command_visitor: &mut dyn ISerializeCommandVisitor) {
        command_visitor.visit_binary_command(self);
    }
}

pub struct COpndCommand {
    commands: Arc<Vec<i32>>,
    is_negative: bool,
    is_sci_fmt: bool,
    is_decimal: bool,
    is_initialized: bool,
    token: String,
    value: Rational,
}

impl COpndCommand {
    pub fn new(commands: Arc<Vec<i32>>, is_negative: bool, is_decimal: bool, is_sci_fmt: bool) -> Self {
        Self {
            commands,
            is_negative,
            is_sci_fmt,
            is_decimal,
            is_initialized: false,
            token: String::new(),
            value: Rational::default(),
        }
    }

    pub fn initialize(&mut self, rat: Rational) {
        self.value = rat;
        self.is_initialized = true;
    }

    pub fn get_commands(&self) -> Arc<Vec<i32>> {
        Arc::clone(&self.commands)
    }

    pub fn set_commands(&mut self, commands: Arc<Vec<i32>>) {
        self.commands = commands;
    }

    pub fn append_command(&mut self, command: i32) {
        if self.is_sci_fmt {
            self.clear_all_and_append_command(command);
        } else {
            Arc::make_mut(&mut self.commands).push(command);
        }

        if command == IDC_PNT {
            self.is_decimal = true;
        }
    }

    pub fn toggle_sign(&mut self) {
        for &n_op_code in self.commands.iter() {
            if n_op_code != IDC_0 {
                self.is_negative = !self.is_negative;
                break;
            }
        }
    }

    pub fn remove_from_end(&mut self) {
        if self.is_sci_fmt {
            self.clear_all_and_append_command(IDC_0);
        } else {
            let n_commands = self.commands.len();

            if n_commands == 1 {
                self.clear_all_and_append_command(IDC_0);
            } else {
                let n_op_code = self.commands[n_commands - 1];

                if n_op_code == IDC_PNT {
                    self.is_decimal = false;
                }

                Arc::make_mut(&mut self.commands).pop();
            }
        }
    }

    pub fn is_negative(&self) -> bool {
        self.is_negative
    }

    pub fn is_sci_fmt(&self) -> bool {
        self.is_sci_fmt
    }

    pub fn is_decimal_present(&self) -> bool {
        self.is_decimal
    }

    pub fn get_command_type(&self) -> CommandType {
        CommandType::OperandCommand
    }

    pub fn clear_all_and_append_command(&mut self, command: i32) {
        Arc::make_mut(&mut self.commands).clear();
        Arc::make_mut(&mut self.commands).push(command);
        self.is_sci_fmt = false;
        self.is_negative = false;
        self.is_decimal = false;
    }

    pub fn get_token(&mut self, decimal_symbol: char) -> &str {
        const CH_ZERO: char = '0';
        const CH_EXP: char = 'e';
        const CH_NEGATE: char = '-';
        const CH_PLUS: char = '+';

        self.token.clear();

        for &n_op_code in self.commands.iter() {
            match n_op_code {
                IDC_PNT => self.token.push(decimal_symbol),
                IDC_EXP => {
                    self.token.push(CH_EXP);
                    if self.commands.iter().position(|&x| x == IDC_SIGN).is_none() {
                        self.token.push(CH_PLUS);
                    }
                }
                IDC_SIGN => self.token.push(CH_NEGATE),
                _ => self.token.push_str(&((n_op_code - IDC_0).to_string())),
            }
        }

        for (i, ch) in self.token.chars().enumerate() {
            if ch != CH_ZERO {
                if ch == decimal_symbol {
                    self.token.drain(0..i - 1);
                } else {
                    self.token.drain(0..i);
                }

                if self.is_negative {
                    self.token.insert(0, CH_NEGATE);
                }

                return &self.token;
            }
        }

        self.token = CH_ZERO.to_string();
        &self.token
    }

    pub fn get_string(&self, radix: u32, precision: i32) -> String {
        if self.is_initialized {
            self.value.to_string(radix, NumberFormat::Float, precision)
        } else {
            String::new()
        }
    }

    pub fn accept(&self, command_visitor: &mut dyn ISerializeCommandVisitor) {
        command_visitor.visit_opnd_command(self);
    }
}

pub trait ISerializeCommandVisitor {
    fn visit_opnd_command(&mut self, opnd_cmd: &COpndCommand);
    fn visit_unary_command(&mut self, unary_cmd: &CUnaryCommand);
    fn visit_binary_command(&mut self, binary_cmd: &CBinaryCommand);
    fn visit_parentheses(&mut self, para_cmd: &CParentheses);
}

pub enum CommandType {
    Parentheses,
    UnaryCommand,
    BinaryCommand,
    OperandCommand,
}

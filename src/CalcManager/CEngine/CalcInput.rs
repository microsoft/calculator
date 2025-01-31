use std::cmp::Ordering;

const MAX_STRLEN: usize = 84;
const C_NUM_MAX_DIGITS: usize = MAX_STRLEN;
const C_EXP_MAX_DIGITS: usize = 4;

pub struct CalcNumSec {
    value: String,
    is_negative: bool,
}

impl CalcNumSec {
    pub fn new() -> Self {
        Self {
            value: String::new(),
            is_negative: false,
        }
    }

    pub fn clear(&mut self) {
        self.value.clear();
        self.is_negative = false;
    }

    pub fn is_empty(&self) -> bool {
        self.value.is_empty()
    }

    pub fn is_negative(&self) -> bool {
        self.is_negative
    }

    pub fn set_negative(&mut self, is_negative: bool) {
        self.is_negative = is_negative;
    }
}

pub struct CalcInput {
    base: CalcNumSec,
    exponent: CalcNumSec,
    has_exponent: bool,
    has_decimal: bool,
    dec_pt_index: usize,
    dec_symbol: char,
}

impl CalcInput {
    pub fn new(dec_symbol: char) -> Self {
        Self {
            base: CalcNumSec::new(),
            exponent: CalcNumSec::new(),
            has_exponent: false,
            has_decimal: false,
            dec_pt_index: 0,
            dec_symbol,
        }
    }

    pub fn clear(&mut self) {
        self.base.clear();
        self.exponent.clear();
        self.has_exponent = false;
        self.has_decimal = false;
        self.dec_pt_index = 0;
    }

    pub fn try_toggle_sign(&mut self, is_integer_mode: bool, max_num_str: &str) -> bool {
        if self.base.is_empty() {
            self.base.set_negative(false);
            self.exponent.set_negative(false);
        } else if self.has_exponent {
            self.exponent.set_negative(!self.exponent.is_negative());
        } else {
            if is_integer_mode && self.base.is_negative() {
                if self.base.value.len() >= max_num_str.len() && self.base.value.chars().last() > max_num_str.chars().last() {
                    return false;
                }
            }
            self.base.set_negative(!self.base.is_negative());
        }
        true
    }

    pub fn try_add_digit(&mut self, value: u32, radix: u32, is_integer_mode: bool, max_num_str: &str, word_bit_width: i32, max_digits: usize) -> bool {
        let ch_digit = if value < 10 {
            (b'0' + value as u8) as char
        } else {
            (b'A' + value as u8 - 10) as char
        };

        let (p_num_sec, max_count) = if self.has_exponent {
            (&mut self.exponent, C_EXP_MAX_DIGITS)
        } else {
            let mut max_count = max_digits;
            if self.has_decimal {
                max_count += 1;
            }
            if !self.base.is_empty() && self.base.value.starts_with('0') {
                max_count += 1;
            }
            (&mut self.base, max_count)
        };

        if p_num_sec.is_empty() && value == 0 {
            return true;
        }

        if p_num_sec.value.len() < max_count {
            p_num_sec.value.push(ch_digit);
            return true;
        }

        if is_integer_mode && p_num_sec.value.len() == max_count && !self.has_exponent {
            let allow_extra_digit = match radix {
                8 => match word_bit_width % 3 {
                    1 => p_num_sec.value.starts_with('1'),
                    2 => p_num_sec.value.starts_with(|c| c <= '3'),
                    _ => false,
                },
                10 => {
                    if p_num_sec.value.len() < max_num_str.len() {
                        match p_num_sec.value.cmp(&max_num_str[..p_num_sec.value.len()]) {
                            Ordering::Less => true,
                            Ordering::Equal => {
                                let last_char = max_num_str.chars().nth(p_num_sec.value.len()).unwrap();
                                ch_digit <= last_char || (p_num_sec.is_negative() && ch_digit <= last_char + 1)
                            }
                            Ordering::Greater => false,
                        }
                    } else {
                        false
                    }
                }
                _ => false,
            };

            if allow_extra_digit {
                p_num_sec.value.push(ch_digit);
                return true;
            }
        }

        false
    }

    pub fn try_add_decimal_pt(&mut self) -> bool {
        if self.has_decimal || self.has_exponent {
            return false;
        }

        if self.base.is_empty() {
            self.base.value.push('0');
        }

        self.dec_pt_index = self.base.value.len();
        self.base.value.push(self.dec_symbol);
        self.has_decimal = true;

        true
    }

    pub fn has_decimal_pt(&self) -> bool {
        self.has_decimal
    }

    pub fn try_begin_exponent(&mut self) -> bool {
        self.try_add_decimal_pt();

        if self.has_exponent {
            return false;
        }

        self.has_exponent = true;
        true
    }

    pub fn backspace(&mut self) {
        if self.has_exponent {
            if !self.exponent.is_empty() {
                self.exponent.value.pop();
                if self.exponent.is_empty() {
                    self.exponent.clear();
                }
            } else {
                self.has_exponent = false;
            }
        } else {
            if !self.base.is_empty() {
                self.base.value.pop();
                if self.base.value == "0" {
                    self.base.value.pop();
                }
            }

            if self.base.value.len() <= self.dec_pt_index {
                self.has_decimal = false;
                self.dec_pt_index = 0;
            }

            if self.base.is_empty() {
                self.base.clear();
            }
        }
    }

    pub fn set_decimal_symbol(&mut self, dec_symbol: char) {
        if self.dec_symbol != dec_symbol {
            self.dec_symbol = dec_symbol;

            if self.has_decimal {
                self.base.value.replace_range(self.dec_pt_index..=self.dec_pt_index, &dec_symbol.to_string());
            }
        }
    }

    pub fn is_empty(&self) -> bool {
        self.base.is_empty() && !self.has_exponent && self.exponent.is_empty() && !self.has_decimal
    }

    pub fn to_string(&self, radix: u32) -> String {
        if self.base.value.len() > MAX_STRLEN || (self.has_exponent && self.exponent.value.len() > MAX_STRLEN) {
            return String::new();
        }

        let mut result = String::new();

        if self.base.is_negative() {
            result.push('-');
        }

        if self.base.is_empty() {
            result.push('0');
        } else {
            result.push_str(&self.base.value);
        }

        if self.has_exponent {
            if !self.has_decimal {
                result.push(self.dec_symbol);
            }

            result.push(if radix == 10 { 'e' } else { '^' });
            result.push(if self.exponent.is_negative() { '-' } else { '+' });

            if self.exponent.is_empty() {
                result.push('0');
            } else {
                result.push_str(&self.exponent.value);
            }
        }

        if result.len() > C_NUM_MAX_DIGITS * 2 + 4 {
            return String::new();
        }

        result
    }

    pub fn to_rational(&self, radix: u32, precision: i32) -> Rational {
        let rat = string_to_rat(self.base.is_negative(), &self.base.value, self.exponent.is_negative(), &self.exponent.value, radix, precision);
        if rat.is_none() {
            return Rational::default();
        }

        let result = Rational::from_rat(rat.unwrap());
        destroy_rat(rat.unwrap());

        result
    }
}

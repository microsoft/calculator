use std::collections::HashMap;
use std::sync::Mutex;
use regex::Regex;

lazy_static! {
    static ref ENGINE_STRINGS: Mutex<HashMap<&'static str, String>> = Mutex::new(HashMap::new());
}

const MAX_EXPONENT: usize = 4;
const MAX_GROUPING_SIZE: u32 = 16;
const DEC_PRE_SEP_STR: &str = "[+-]?(\\d*)[";
const DEC_POST_SEP_STR: &str = "]?(\\d*)(?:e[+-]?(\\d*))?$";

#[derive(Clone, Copy, PartialEq)]
struct LastDisp {
    value: Rational,
    precision: i32,
    radix: u32,
    nfe: i32,
    numwidth: NumWidth,
    int_math: bool,
    record: bool,
    use_sep: bool,
}

static mut GLD_PREVIOUS: LastDisp = LastDisp {
    value: Rational::new(0, 1),
    precision: -1,
    radix: 0,
    nfe: -1,
    numwidth: NumWidth::Qword,
    int_math: false,
    record: false,
    use_sep: false,
};

impl CCalcEngine {
    fn truncate_num_for_int_math(&self, rat: Rational) -> Rational {
        if !self.integer_mode {
            return rat;
        }

        let result = RationalMath::integer(rat);

        if result < Rational::new(0, 1) {
            let mut result = -(result) - Rational::new(1, 1);
            result ^= self.get_chop_number();
            return result;
        }

        result & self.get_chop_number()
    }

    fn display_num(&mut self) {
        unsafe {
            if self.record
                || GLD_PREVIOUS.value != self.current_val
                || GLD_PREVIOUS.precision != self.precision
                || GLD_PREVIOUS.radix != self.radix
                || GLD_PREVIOUS.nfe != self.nfe
                || !GLD_PREVIOUS.use_sep
                || GLD_PREVIOUS.numwidth != self.numwidth
                || GLD_PREVIOUS.int_math != self.integer_mode
                || GLD_PREVIOUS.record != self.record
            {
                GLD_PREVIOUS.precision = self.precision;
                GLD_PREVIOUS.radix = self.radix;
                GLD_PREVIOUS.nfe = self.nfe;
                GLD_PREVIOUS.numwidth = self.numwidth;
                GLD_PREVIOUS.int_math = self.integer_mode;
                GLD_PREVIOUS.record = self.record;
                GLD_PREVIOUS.use_sep = true;

                if self.record {
                    self.number_string = self.input.to_string(self.radix);
                } else {
                    if self.integer_mode {
                        self.current_val = self.truncate_num_for_int_math(self.current_val);
                    }
                    self.number_string = self.get_string_for_display(self.current_val, self.radix);
                }

                GLD_PREVIOUS.value = self.current_val;

                if self.radix == 10 && self.is_number_invalid(&self.number_string, MAX_EXPONENT, self.precision, self.radix) {
                    self.display_error(CALC_E_OVERFLOW);
                } else {
                    self.set_primary_display(self.group_digits_per_radix(&self.number_string, self.radix));
                }
            }
        }
    }

    fn is_number_invalid(&self, number_string: &str, max_exp: usize, max_mantissa: i32, radix: u32) -> bool {
        if radix == 10 {
            let regex_str = format!("{}{}{}", DEC_PRE_SEP_STR, self.decimal_separator, DEC_POST_SEP_STR);
            let re = Regex::new(&regex_str).unwrap();
            if let Some(caps) = re.captures(number_string) {
                if caps.get(3).map_or(0, |m| m.as_str().len()) > max_exp {
                    return true;
                } else {
                    let exp = caps.get(1).map_or("", |m| m.as_str());
                    let int_itr = exp.chars().skip_while(|&c| c == '0');
                    let i_mantissa = int_itr.clone().count() + caps.get(2).map_or(0, |m| m.as_str().len());
                    if i_mantissa > max_mantissa as usize {
                        return true;
                    }
                }
            } else {
                return true;
            }
        } else {
            for c in number_string.chars() {
                if radix == 16 {
                    if !c.is_digit(16) {
                        return true;
                    }
                } else if c < '0' || c >= (b'0' + radix as u8) as char {
                    return true;
                }
            }
        }

        false
    }

    fn digit_grouping_string_to_grouping_vector(grouping_string: &str) -> Vec<u32> {
        let mut grouping = Vec::new();
        let mut current_group = 0;
        let mut next = grouping_string;
        while let Some((group, rest)) = next.split_once(';') {
            current_group = group.parse().unwrap_or(0);
            if current_group < MAX_GROUPING_SIZE {
                grouping.push(current_group);
            }
            next = rest;
        }
        current_group = next.parse().unwrap_or(0);
        if current_group < MAX_GROUPING_SIZE {
            grouping.push(current_group);
        }
        grouping
    }

    fn group_digits_per_radix(&self, number_string: &str, radix: u32) -> String {
        if number_string.is_empty() {
            return String::new();
        }

        match radix {
            10 => self.group_digits(&self.group_separator, &self.dec_grouping, number_string, number_string.starts_with('-')),
            8 => self.group_digits(" ", &[3, 0], number_string, false),
            2 | 16 => self.group_digits(" ", &[4, 0], number_string, false),
            _ => number_string.to_string(),
        }
    }

    fn group_digits(&self, delimiter: &str, grouping: &[u32], display_string: &str, is_num_negative: bool) -> String {
        if delimiter.is_empty() || grouping.is_empty() {
            return display_string.to_string();
        }

        let exp = display_string.find('e');
        let has_exponent = exp.is_some();

        let dec = display_string.find(self.decimal_separator);
        let has_decimal = dec.is_some();

        let ritr = if has_decimal {
            display_string[..dec.unwrap()].chars().rev()
        } else if has_exponent {
            display_string[..exp.unwrap()].chars().rev()
        } else {
            display_string.chars().rev()
        };

        let mut result = String::new();
        let mut grouping_size = 0;

        let mut group_itr = grouping.iter();
        let mut curr_grouping = *group_itr.next().unwrap_or(&0);

        for c in ritr {
            result.push(c);
            grouping_size += 1;

            if curr_grouping != 0 && grouping_size % curr_grouping == 0 {
                result.push_str(delimiter);
                grouping_size = 0;

                if let Some(&next_group) = group_itr.next() {
                    curr_grouping = next_group;
                }
            }
        }

        if is_num_negative {
            result.push(display_string.chars().next().unwrap());
        }

        result.chars().rev().collect::<String>() + &display_string[dec.unwrap_or(display_string.len())..]
    }
}

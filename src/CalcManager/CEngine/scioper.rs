use crate::calc_engine::CalcEngine;
use crate::rational::Rational;
use crate::rational_math::{self, RationalMath};
use crate::winerror_cross_platform::CALC_E_NORESULT;

impl CalcEngine {
    pub fn do_operation(&mut self, operation: i32, lhs: Rational, rhs: Rational) -> Rational {
        let mut result = if lhs != Rational::default() { lhs } else { Rational::default() };

        match operation {
            IDC_AND => result &= rhs,
            IDC_OR => result |= rhs,
            IDC_XOR => result ^= rhs,
            IDC_NAND => result = (result & rhs) ^ self.get_chop_number(),
            IDC_NOR => result = (result | rhs) ^ self.get_chop_number(),
            IDC_RSHF => {
                if self.integer_mode && result >= self.word_bit_width {
                    return self.display_error(CALC_E_NORESULT);
                }

                let mut w64_bits = rhs.to_u64();
                let f_msb = (w64_bits >> (self.word_bit_width - 1)) & 1;

                let hold_val = result;
                result = rhs >> hold_val;

                if f_msb != 0 {
                    result = RationalMath::integer(result);

                    let mut temp_rat = self.get_chop_number() >> hold_val;
                    temp_rat = RationalMath::integer(temp_rat);

                    result |= temp_rat ^ self.get_chop_number();
                }
            }
            IDC_RSHFL => {
                if self.integer_mode && result >= self.word_bit_width {
                    return self.display_error(CALC_E_NORESULT);
                }

                result = rhs >> result;
            }
            IDC_LSHF => {
                if self.integer_mode && result >= self.word_bit_width {
                    return self.display_error(CALC_E_NORESULT);
                }

                result = rhs << result;
            }
            IDC_ADD => result += rhs,
            IDC_SUB => result = rhs - result,
            IDC_MUL => result *= rhs,
            IDC_DIV | IDC_MOD => {
                let mut i_numerator_sign = 1;
                let mut i_denominator_sign = 1;
                let mut temp = result;
                result = rhs;

                if self.integer_mode {
                    let mut w64_bits = rhs.to_u64();
                    let f_msb = (w64_bits >> (self.word_bit_width - 1)) & 1;

                    if f_msb != 0 {
                        result = (rhs ^ self.get_chop_number()) + 1;
                        i_numerator_sign = -1;
                    }

                    w64_bits = temp.to_u64();
                    let f_msb = (w64_bits >> (self.word_bit_width - 1)) & 1;

                    if f_msb != 0 {
                        temp = (temp ^ self.get_chop_number()) + 1;
                        i_denominator_sign = -1;
                    }
                }

                if operation == IDC_DIV {
                    result /= temp;
                    if self.integer_mode && (i_numerator_sign * i_denominator_sign) == -1 {
                        result = -RationalMath::integer(result);
                    }
                } else {
                    if self.integer_mode {
                        result %= temp;
                        if i_numerator_sign == -1 {
                            result = -RationalMath::integer(result);
                        }
                    } else {
                        result = RationalMath::modulus(result, temp);
                    }
                }
            }
            IDC_PWR => result = RationalMath::pow(rhs, result),
            IDC_ROOT => result = RationalMath::root(rhs, result),
            IDC_LOGBASEY => result = RationalMath::log(rhs) / RationalMath::log(result),
            _ => {}
        }

        result
    }
}

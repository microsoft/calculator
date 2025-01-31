use crate::calc_engine::CalcEngine;
use crate::rational::Rational;
use crate::rational_math::{self, RationalMath};
use crate::winerror_cross_platform::CALC_E_DOMAIN;
use std::ops::Neg;

impl CalcEngine {
    pub fn sci_calc_functions(&mut self, rat: Rational, op: u32) -> Rational {
        let mut result = Rational::default();
        match op {
            IDC_CHOP => {
                result = if self.inv {
                    RationalMath::frac(rat)
                } else {
                    RationalMath::integer(rat)
                };
            }
            IDC_COM => {
                if self.radix == 10 && !self.integer_mode {
                    result = -(RationalMath::integer(rat) + 1);
                } else {
                    result = rat ^ self.get_chop_number();
                }
            }
            IDC_ROL | IDC_ROLC => {
                if self.integer_mode {
                    result = RationalMath::integer(rat);
                    let mut w64_bits = result.to_u64();
                    let msb = (w64_bits >> (self.word_bit_width - 1)) & 1;
                    w64_bits <<= 1;
                    if op == IDC_ROL {
                        w64_bits |= msb;
                    } else {
                        w64_bits |= self.carry_bit;
                        self.carry_bit = msb;
                    }
                    result = Rational::from_u64(w64_bits);
                }
            }
            IDC_ROR | IDC_RORC => {
                if self.integer_mode {
                    result = RationalMath::integer(rat);
                    let mut w64_bits = result.to_u64();
                    let lsb = (w64_bits & 0x01) == 1;
                    w64_bits >>= 1;
                    if op == IDC_ROR {
                        w64_bits |= (lsb as u64) << (self.word_bit_width - 1);
                    } else {
                        w64_bits |= (self.carry_bit as u64) << (self.word_bit_width - 1);
                        self.carry_bit = lsb as u64;
                    }
                    result = Rational::from_u64(w64_bits);
                }
            }
            IDC_PERCENT => {
                if self.op_code == IDC_MUL || self.op_code == IDC_DIV {
                    result = rat / 100;
                } else {
                    result = rat * (self.last_val / 100);
                }
            }
            IDC_SIN => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::asin(rat, self.angle_type)
                    } else {
                        RationalMath::sin(rat, self.angle_type)
                    };
                }
            }
            IDC_SINH => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::asinh(rat)
                    } else {
                        RationalMath::sinh(rat)
                    };
                }
            }
            IDC_COS => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::acos(rat, self.angle_type)
                    } else {
                        RationalMath::cos(rat, self.angle_type)
                    };
                }
            }
            IDC_COSH => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::acosh(rat)
                    } else {
                        RationalMath::cosh(rat)
                    };
                }
            }
            IDC_TAN => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::atan(rat, self.angle_type)
                    } else {
                        RationalMath::tan(rat, self.angle_type)
                    };
                }
            }
            IDC_TANH => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::atanh(rat)
                    } else {
                        RationalMath::tanh(rat)
                    };
                }
            }
            IDC_SEC => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::acos(RationalMath::invert(rat), self.angle_type)
                    } else {
                        RationalMath::invert(RationalMath::cos(rat, self.angle_type))
                    };
                }
            }
            IDC_CSC => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::asin(RationalMath::invert(rat), self.angle_type)
                    } else {
                        RationalMath::invert(RationalMath::sin(rat, self.angle_type))
                    };
                }
            }
            IDC_COT => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::atan(RationalMath::invert(rat), self.angle_type)
                    } else {
                        RationalMath::invert(RationalMath::tan(rat, self.angle_type))
                    };
                }
            }
            IDC_SECH => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::acosh(RationalMath::invert(rat))
                    } else {
                        RationalMath::invert(RationalMath::cosh(rat))
                    };
                }
            }
            IDC_CSCH => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::asinh(RationalMath::invert(rat))
                    } else {
                        RationalMath::invert(RationalMath::sinh(rat))
                    };
                }
            }
            IDC_COTH => {
                if !self.integer_mode {
                    result = if self.inv {
                        RationalMath::atanh(RationalMath::invert(rat))
                    } else {
                        RationalMath::invert(RationalMath::tanh(rat))
                    };
                }
            }
            IDC_REC => {
                result = RationalMath::invert(rat);
            }
            IDC_SQR => {
                result = RationalMath::pow(rat, 2);
            }
            IDC_SQRT => {
                result = RationalMath::root(rat, 2);
            }
            IDC_CUBEROOT | IDC_CUB => {
                result = if op == IDC_CUBEROOT {
                    RationalMath::root(rat, 3)
                } else {
                    RationalMath::pow(rat, 3)
                };
            }
            IDC_LOG => {
                result = RationalMath::log10(rat);
            }
            IDC_POW10 => {
                result = RationalMath::pow(10, rat);
            }
            IDC_POW2 => {
                result = RationalMath::pow(2, rat);
            }
            IDC_LN => {
                result = if self.inv {
                    RationalMath::exp(rat)
                } else {
                    RationalMath::log(rat)
                };
            }
            IDC_FAC => {
                result = RationalMath::fact(rat);
            }
            IDC_DEGREES => {
                self.process_command(IDC_INV);
                // This case falls through to IDC_DMS case because in the old Win32 Calc,
                // the degrees functionality was achieved as 'Inv' of 'dms' operation,
                // so setting the IDC_INV command first and then performing 'dms' operation as global variables m_bInv, m_bRecord
                // are set properly through process_command(IDC_INV)
                result = self.sci_calc_functions(rat, IDC_DMS);
            }
            IDC_DMS => {
                if !self.integer_mode {
                    let shft_rat = if self.inv { 100 } else { 60 };
                    let degree_rat = RationalMath::integer(rat);
                    let mut minute_rat = (rat - degree_rat) * shft_rat;
                    let mut second_rat = minute_rat;
                    minute_rat = RationalMath::integer(minute_rat);
                    second_rat = (second_rat - minute_rat) * shft_rat;
                    let shft_rat = if self.inv { 60 } else { 100 };
                    second_rat /= shft_rat;
                    minute_rat = (minute_rat + second_rat) / shft_rat;
                    result = degree_rat + minute_rat;
                }
            }
            IDC_CEIL => {
                result = if RationalMath::frac(rat) > 0 {
                    RationalMath::integer(rat + 1)
                } else {
                    RationalMath::integer(rat)
                };
            }
            IDC_FLOOR => {
                result = if RationalMath::frac(rat) < 0 {
                    RationalMath::integer(rat - 1)
                } else {
                    RationalMath::integer(rat)
                };
            }
            IDC_ABS => {
                result = RationalMath::abs(rat);
            }
            _ => {}
        }
        result
    }

    pub fn display_error(&mut self, n_error: u32) {
        let error_string = self.get_string(IDS_ERRORS_FIRST + SCODE_CODE(n_error));
        self.set_primary_display(&error_string, true);
        self.error = true;
        self.history_collector.clear_history_line(&error_string);
    }
}

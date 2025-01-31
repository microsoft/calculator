use std::cmp::Ordering;

pub struct NumberFormattingUtils;

impl NumberFormattingUtils {
    pub fn format_number(number: &Rational, radix: u32, precision: i32) -> String {
        number.to_string(radix, NumberFormat::Normal, precision)
    }

    pub fn format_number_scientific(number: &Rational, radix: u32, precision: i32) -> String {
        number.to_string(radix, NumberFormat::Scientific, precision)
    }

    pub fn format_number_engineering(number: &Rational, radix: u32, precision: i32) -> String {
        number.to_string(radix, NumberFormat::Engineering, precision)
    }

    pub fn format_number_fixed(number: &Rational, radix: u32, precision: i32) -> String {
        number.to_string(radix, NumberFormat::Fixed, precision)
    }

    pub fn format_number_currency(number: &Rational, radix: u32, precision: i32) -> String {
        number.to_string(radix, NumberFormat::Currency, precision)
    }

    pub fn format_number_percent(number: &Rational, radix: u32, precision: i32) -> String {
        number.to_string(radix, NumberFormat::Percent, precision)
    }

    pub fn format_number_fraction(number: &Rational, radix: u32, precision: i32) -> String {
        number.to_string(radix, NumberFormat::Fraction, precision)
    }

    pub fn format_number_custom(number: &Rational, radix: u32, precision: i32, format: NumberFormat) -> String {
        number.to_string(radix, format, precision)
    }
}

use std::cmp::Ordering;

pub struct Rational {
    p: Number,
    q: Number,
}

impl Rational {
    pub fn new() -> Self {
        Self {
            p: Number::new(),
            q: Number::with_values(1, 0, vec![1]),
        }
    }

    pub fn from_number(n: &Number) -> Self {
        let mut q_exp = 0;
        if n.exp() < 0 {
            q_exp -= n.exp();
        }

        Self {
            p: Number::with_values(n.sign(), 0, n.mantissa().clone()),
            q: Number::with_values(1, q_exp, vec![1]),
        }
    }

    pub fn with_values(p: Number, q: Number) -> Self {
        Self { p, q }
    }

    pub fn from_i32(i: i32) -> Self {
        let prat = i32torat(i);
        let p = Number::from_pnumber(&prat.pp);
        let q = Number::from_pnumber(&prat.pq);
        destroyrat(prat);
        Self { p, q }
    }

    pub fn from_u32(ui: u32) -> Self {
        let prat = ui32torat(ui);
        let p = Number::from_pnumber(&prat.pp);
        let q = Number::from_pnumber(&prat.pq);
        destroyrat(prat);
        Self { p, q }
    }

    pub fn from_u64(ui: u64) -> Self {
        let hi = (ui >> 32) as u32;
        let lo = ui as u32;
        let temp = Rational::from_u32(hi) << 32 | Rational::from_u32(lo);
        Self {
            p: temp.p,
            q: temp.q,
        }
    }

    pub fn from_prat(prat: &PRAT) -> Self {
        Self {
            p: Number::from_pnumber(&prat.pp),
            q: Number::from_pnumber(&prat.pq),
        }
    }

    pub fn to_prat(&self) -> PRAT {
        let mut ret = PRAT::new();
        ret.pp = self.p.to_pnumber();
        ret.pq = self.q.to_pnumber();
        ret
    }

    pub fn p(&self) -> &Number {
        &self.p
    }

    pub fn q(&self) -> &Number {
        &self.q
    }

    pub fn negate(&self) -> Self {
        Self {
            p: Number::with_values(-self.p.sign(), self.p.exp(), self.p.mantissa().clone()),
            q: self.q.clone(),
        }
    }

    pub fn add_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = rhs.to_prat();
        addrat(&mut lhs_rat, &rhs_rat, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn sub_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = rhs.to_prat();
        subrat(&mut lhs_rat, &rhs_rat, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn mul_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = rhs.to_prat();
        mulrat(&mut lhs_rat, &rhs_rat, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn div_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = rhs.to_prat();
        divrat(&mut lhs_rat, &rhs_rat, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn rem_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = rhs.to_prat();
        remrat(&mut lhs_rat, &rhs_rat);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn shl_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = self.to_prat();
        lshrat(&mut lhs_rat, &rhs_rat, RATIONAL_BASE, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn shr_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = self.to_prat();
        rshrat(&mut lhs_rat, &rhs_rat, RATIONAL_BASE, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn and_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = self.to_prat();
        andrat(&mut lhs_rat, &rhs_rat, RATIONAL_BASE, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn or_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = self.to_prat();
        orrat(&mut lhs_rat, &rhs_rat, RATIONAL_BASE, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn xor_assign(&mut self, rhs: &Self) {
        let mut lhs_rat = self.to_prat();
        let rhs_rat = self.to_prat();
        xorrat(&mut lhs_rat, &rhs_rat, RATIONAL_BASE, RATIONAL_PRECISION);
        destroyrat(rhs_rat);
        *self = Rational::from_prat(&lhs_rat);
        destroyrat(lhs_rat);
    }

    pub fn to_string(&self, radix: u32, fmt: NumberFormat, precision: i32) -> String {
        let rat = self.to_prat();
        let result = rat_to_string(&rat, fmt, radix, precision);
        destroyrat(rat);
        result
    }

    pub fn to_u64(&self) -> u64 {
        let rat = self.to_prat();
        let result = rat_to_u64(&rat, RATIONAL_BASE, RATIONAL_PRECISION);
        destroyrat(rat);
        result
    }
}

impl PartialEq for Rational {
    fn eq(&self, other: &Self) -> bool {
        let lhs_rat = self.to_prat();
        let rhs_rat = other.to_prat();
        let result = rat_equ(&lhs_rat, &rhs_rat, RATIONAL_PRECISION);
        destroyrat(lhs_rat);
        destroyrat(rhs_rat);
        result
    }
}

impl Eq for Rational {}

impl PartialOrd for Rational {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Rational {
    fn cmp(&self, other: &Self) -> Ordering {
        let lhs_rat = self.to_prat();
        let rhs_rat = other.to_prat();
        let result = rat_cmp(&lhs_rat, &rhs_rat, RATIONAL_PRECISION);
        destroyrat(lhs_rat);
        destroyrat(rhs_rat);
        result
    }
}

impl std::ops::AddAssign for Rational {
    fn add_assign(&mut self, rhs: Self) {
        self.add_assign(&rhs);
    }
}

impl std::ops::SubAssign for Rational {
    fn sub_assign(&mut self, rhs: Self) {
        self.sub_assign(&rhs);
    }
}

impl std::ops::MulAssign for Rational {
    fn mul_assign(&mut self, rhs: Self) {
        self.mul_assign(&rhs);
    }
}

impl std::ops::DivAssign for Rational {
    fn div_assign(&mut self, rhs: Self) {
        self.div_assign(&rhs);
    }
}

impl std::ops::RemAssign for Rational {
    fn rem_assign(&mut self, rhs: Self) {
        self.rem_assign(&rhs);
    }
}

impl std::ops::ShlAssign for Rational {
    fn shl_assign(&mut self, rhs: Self) {
        self.shl_assign(&rhs);
    }
}

impl std::ops::ShrAssign for Rational {
    fn shr_assign(&mut self, rhs: Self) {
        self.shr_assign(&rhs);
    }
}

impl std::ops::BitAndAssign for Rational {
    fn bitand_assign(&mut self, rhs: Self) {
        self.and_assign(&rhs);
    }
}

impl std::ops::BitOrAssign for Rational {
    fn bitor_assign(&mut self, rhs: Self) {
        self.or_assign(&rhs);
    }
}

impl std::ops::BitXorAssign for Rational {
    fn bitxor_assign(&mut self, rhs: Self) {
        self.xor_assign(&rhs);
    }
}

impl std::ops::Neg for Rational {
    type Output = Self;

    fn neg(self) -> Self::Output {
        self.negate()
    }
}

impl std::ops::Add for Rational {
    type Output = Self;

    fn add(mut self, rhs: Self) -> Self::Output {
        self.add_assign(rhs);
        self
    }
}

impl std::ops::Sub for Rational {
    type Output = Self;

    fn sub(mut self, rhs: Self) -> Self::Output {
        self.sub_assign(rhs);
        self
    }
}

impl std::ops::Mul for Rational {
    type Output = Self;

    fn mul(mut self, rhs: Self) -> Self::Output {
        self.mul_assign(rhs);
        self
    }
}

impl std::ops::Div for Rational {
    type Output = Self;

    fn div(mut self, rhs: Self) -> Self::Output {
        self.div_assign(rhs);
        self
    }
}

impl std::ops::Rem for Rational {
    type Output = Self;

    fn rem(mut self, rhs: Self) -> Self::Output {
        self.rem_assign(rhs);
        self
    }
}

impl std::ops::Shl for Rational {
    type Output = Self;

    fn shl(mut self, rhs: Self) -> Self::Output {
        self.shl_assign(rhs);
        self
    }
}

impl std::ops::Shr for Rational {
    type Output = Self;

    fn shr(mut self, rhs: Self) -> Self::Output {
        self.shr_assign(rhs);
        self
    }
}

impl std::ops::BitAnd for Rational {
    type Output = Self;

    fn bitand(mut self, rhs: Self) -> Self::Output {
        self.and_assign(rhs);
        self
    }
}

impl std::ops::BitOr for Rational {
    type Output = Self;

    fn bitor(mut self, rhs: Self) -> Self::Output {
        self.or_assign(rhs);
        self
    }
}

impl std::ops::BitXor for Rational {
    type Output = Self;

    fn bitxor(mut self, rhs: Self) -> Self::Output {
        self.xor_assign(rhs);
        self
    }
}

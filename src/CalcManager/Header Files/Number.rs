pub struct Number {
    sign: i32,
    exp: i32,
    mantissa: Vec<u32>,
}

impl Number {
    pub fn new() -> Self {
        Self {
            sign: 1,
            exp: 0,
            mantissa: vec![0],
        }
    }

    pub fn with_values(sign: i32, exp: i32, mantissa: Vec<u32>) -> Self {
        Self { sign, exp, mantissa }
    }

    pub fn from_pnumber(p: &PNUMBER) -> Self {
        let mut mantissa = Vec::with_capacity(p.cdigit as usize);
        mantissa.extend_from_slice(&p.mant[..p.cdigit as usize]);
        Self {
            sign: p.sign,
            exp: p.exp,
            mantissa,
        }
    }

    pub fn to_pnumber(&self) -> PNUMBER {
        let mut ret = PNUMBER::new(self.mantissa.len() + 1);
        ret.sign = self.sign;
        ret.exp = self.exp;
        ret.cdigit = self.mantissa.len() as i32;
        ret.mant[..self.mantissa.len()].copy_from_slice(&self.mantissa);
        ret
    }

    pub fn sign(&self) -> i32 {
        self.sign
    }

    pub fn exp(&self) -> i32 {
        self.exp
    }

    pub fn mantissa(&self) -> &Vec<u32> {
        &self.mantissa
    }

    pub fn is_zero(&self) -> bool {
        self.mantissa.iter().all(|&x| x == 0)
    }
}

pub type OpCode = usize;

pub fn is_op_in_range(op: OpCode, x: u32, y: u32) -> bool {
    (op >= x as usize) && (op <= y as usize)
}

pub fn is_bin_op_code(op_code: OpCode) -> bool {
    is_op_in_range(op_code, IDC_AND, IDC_PWR) || is_op_in_range(op_code, IDC_BINARYEXTENDEDFIRST, IDC_BINARYEXTENDEDLAST)
}

pub fn is_unary_op_code(op_code: OpCode) -> bool {
    is_op_in_range(op_code, IDC_UNARYFIRST, IDC_UNARYLAST) || is_op_in_range(op_code, IDC_UNARYEXTENDEDFIRST, IDC_UNARYEXTENDEDLAST)
}

pub fn is_digit_op_code(op_code: OpCode) -> bool {
    is_op_in_range(op_code, IDC_0, IDC_F)
}

pub fn is_gui_setting_op_code(op_code: OpCode) -> bool {
    if is_op_in_range(op_code, IDM_HEX, IDM_BIN) || is_op_in_range(op_code, IDM_QWORD, IDM_BYTE) || is_op_in_range(op_code, IDM_DEG, IDM_GRAD) {
        return true;
    }

    match op_code {
        IDC_INV | IDC_FE | IDC_MCLEAR | IDC_BACK | IDC_EXP | IDC_STORE | IDC_MPLUS | IDC_MMINUS => true,
        _ => false,
    }
}

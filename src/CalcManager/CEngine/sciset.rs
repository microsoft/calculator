use std::collections::HashMap;
use std::sync::Mutex;

lazy_static! {
    static ref SETTINGS: Mutex<HashMap<&'static str, String>> = Mutex::new(HashMap::new());
}

fn load_settings(resource_provider: &dyn ResourceProvider) {
    let sids = vec![
        "SIDS_DECIMAL_SEPARATOR", "SIDS_GROUP_SEPARATOR", "SIDS_GROUPING", "SIDS_MAX_DIGITS",
        "SIDS_PRECISION", "SIDS_RADIX", "SIDS_NUMBER_FORMAT", "SIDS_ANGLE_TYPE", "SIDS_NUM_WIDTH",
        "SIDS_INTEGER_MODE", "SIDS_RECORD", "SIDS_USE_SEPARATOR"
    ];

    let mut settings = SETTINGS.lock().unwrap();
    for sid in sids {
        if let Some(loc_string) = resource_provider.get_cengine_string(sid) {
            settings.insert(sid, loc_string);
        }
    }
}

fn initial_one_time_only_setup(resource_provider: &dyn ResourceProvider) {
    load_settings(resource_provider);
    change_base_constants(DEFAULT_RADIX, DEFAULT_MAX_DIGITS, DEFAULT_PRECISION);
}

fn handle_error_command(idc: OpCode) {
    if !is_gui_setting_op_code(idc) {
        // We would have saved the prev command. Need to forget this state
        TEMP_COM.store(LAST_COM.load(Ordering::SeqCst), Ordering::SeqCst);
    }
}

fn handle_max_digits_reached(calc_display: &dyn CalcDisplay) {
    calc_display.max_digits_reached();
}

fn clear_temporary_values(calc_display: &dyn CalcDisplay) {
    INV.store(false, Ordering::SeqCst);
    INPUT.lock().unwrap().clear();
    RECORD.store(true, Ordering::SeqCst);
    check_and_add_last_bin_op_to_history();
    display_num(calc_display);
    ERROR.store(false, Ordering::SeqCst);
}

fn clear_display(calc_display: &dyn CalcDisplay) {
    calc_display.set_expression_display(vec![], vec![]);
}

fn process_command(calc_display: &dyn CalcDisplay, resource_provider: &dyn ResourceProvider, w_param: OpCode) {
    let mut w_param = w_param;
    if w_param == OpCode::SetResult {
        w_param = OpCode::Recall;
        SET_CALC_STATE.store(true, Ordering::SeqCst);
    }

    process_command_worker(calc_display, resource_provider, w_param);
}

fn process_command_worker(calc_display: &dyn CalcDisplay, resource_provider: &dyn ResourceProvider, w_param: OpCode) {
    if !is_gui_setting_op_code(w_param) {
        LAST_COM.store(TEMP_COM.load(Ordering::SeqCst), Ordering::SeqCst);
        TEMP_COM.store(w_param as i32, Ordering::SeqCst);
    }

    if !NO_PREV_EQU.load(Ordering::SeqCst) {
        clear_display(calc_display);
    }

    if ERROR.load(Ordering::SeqCst) {
        if w_param == OpCode::Clear {
            // handle "C" normally
        } else if w_param == OpCode::Centr {
            // treat "CE" as "C"
            w_param = OpCode::Clear;
        } else {
            handle_error_command(w_param);
            return;
        }
    }

    if RECORD.load(Ordering::SeqCst) {
        if is_bin_op_code(w_param) || is_unary_op_code(w_param) || is_op_in_range(w_param, OpCode::Fe, OpCode::MMinus)
            || is_op_in_range(w_param, OpCode::OpenP, OpCode::CloseP) || is_op_in_range(w_param, OpCode::Hex, OpCode::Bin)
            || is_op_in_range(w_param, OpCode::Qword, OpCode::Byte) || is_op_in_range(w_param, OpCode::Deg, OpCode::Grad)
            || is_op_in_range(w_param, OpCode::BinEditStart, OpCode::BinEditEnd) || w_param == OpCode::Inv
            || (w_param == OpCode::Sign && RADIX.load(Ordering::SeqCst) != 10) || w_param == OpCode::Rand
            || w_param == OpCode::Euler {
            RECORD.store(false, Ordering::SeqCst);
            CURRENT_VAL.store(INPUT.lock().unwrap().to_rational(RADIX.load(Ordering::SeqCst), PRECISION.load(Ordering::SeqCst)), Ordering::SeqCst);
            display_num(calc_display);
        }
    } else if is_digit_op_code(w_param) || w_param == OpCode::Pnt {
        RECORD.store(true, Ordering::SeqCst);
        INPUT.lock().unwrap().clear();
        check_and_add_last_bin_op_to_history();
    }

    if is_digit_op_code(w_param) {
        let i_value = w_param as u32 - OpCode::Digit0 as u32;

        if i_value >= RADIX.load(Ordering::SeqCst) {
            handle_error_command(w_param);
            return;
        }

        if !INPUT.lock().unwrap().try_add_digit(i_value, RADIX.load(Ordering::SeqCst), INTEGER_MODE.load(Ordering::SeqCst), get_max_decimal_value_string(), WORD_BIT_WIDTH.load(Ordering::SeqCst), INT_DIGITS_SAV.load(Ordering::SeqCst)) {
            handle_error_command(w_param);
            handle_max_digits_reached(calc_display);
            return;
        }

        display_num(calc_display);
        return;
    }

    if is_bin_op_code(w_param) {
        if is_bin_op_code(LAST_COM.load(Ordering::SeqCst)) {
            let mut f_prec_inv_to_higher = false;

            OPCODE.store(w_param as i32, Ordering::SeqCst);

            if PRECEDENCE.load(Ordering::SeqCst) && PREV_OPCODE.load(Ordering::SeqCst) != 0 {
                let n_prev = precedence_of_op(PREV_OPCODE.load(Ordering::SeqCst));
                let nx = precedence_of_op(LAST_COM.load(Ordering::SeqCst));
                let ni = precedence_of_op(OPCODE.load(Ordering::SeqCst));
                if nx <= n_prev && ni > n_prev {
                    f_prec_inv_to_higher = true;
                    PREV_OPCODE.store(0, Ordering::SeqCst);
                }
            }
            change_last_bin_op(OPCODE.load(Ordering::SeqCst), f_prec_inv_to_higher, INTEGER_MODE.load(Ordering::SeqCst));
            display_announce_binary_operator(calc_display);
            return;
        }

        if !opnd_added_to_history() {
            add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
        }

        if CHANGE_OP.load(Ordering::SeqCst) {
            loop {
                let nx = precedence_of_op(w_param as i32);
                let ni = precedence_of_op(OPCODE.load(Ordering::SeqCst));

                if nx > ni && PRECEDENCE.load(Ordering::SeqCst) {
                    if PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) < MAX_PREC_DEPTH {
                        PRECEDENCE_VALS.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)] = LAST_VAL.load(Ordering::SeqCst);
                        PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)] = OPCODE.load(Ordering::SeqCst);
                        push_last_opnd_start();
                    } else {
                        PRECEDENCE_OP_COUNT.store(MAX_PREC_DEPTH - 1, Ordering::SeqCst);
                        handle_error_command(w_param);
                    }
                    PRECEDENCE_OP_COUNT.fetch_add(1, Ordering::SeqCst);
                } else {
                    CURRENT_VAL.store(do_operation(OPCODE.load(Ordering::SeqCst), CURRENT_VAL.load(Ordering::SeqCst), LAST_VAL.load(Ordering::SeqCst)), Ordering::SeqCst);
                    PREV_OPCODE.store(OPCODE.load(Ordering::SeqCst), Ordering::SeqCst);

                    if !ERROR.load(Ordering::SeqCst) {
                        display_num(calc_display);
                        if !PRECEDENCE.load(Ordering::SeqCst) {
                            let grouped_string = group_digits_per_radix(NUMBER_STRING.lock().unwrap().clone(), RADIX.load(Ordering::SeqCst));
                            complete_equation(grouped_string);
                            add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
                        }
                    }

                    if PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) != 0 && PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) - 1] != 0 {
                        PRECEDENCE_OP_COUNT.fetch_sub(1, Ordering::SeqCst);
                        OPCODE.store(PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);
                        LAST_VAL.store(PRECEDENCE_VALS.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);
                        let nx = precedence_of_op(OPCODE.load(Ordering::SeqCst));
                        if ni <= nx {
                            enclose_prec_inversion_brackets();
                        }
                        pop_last_opnd_start();
                        continue;
                    }
                }
                break;
            }
        }

        display_announce_binary_operator(calc_display);
        LAST_VAL.store(CURRENT_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
        OPCODE.store(w_param as i32, Ordering::SeqCst);
        add_bin_op_to_history(OPCODE.load(Ordering::SeqCst), INTEGER_MODE.load(Ordering::SeqCst));
        NO_PREV_EQU.store(true, Ordering::SeqCst);
        CHANGE_OP.store(true, Ordering::SeqCst);
        return;
    }

    if is_unary_op_code(w_param) || w_param == OpCode::Degrees {
        if is_bin_op_code(LAST_COM.load(Ordering::SeqCst)) {
            CURRENT_VAL.store(LAST_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
        }

        if w_param != OpCode::Percent {
            if !opnd_added_to_history() {
                add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
            }

            add_unary_op_to_history(w_param as i32, INV.load(Ordering::SeqCst), ANGLE_TYPE.load(Ordering::SeqCst));
        }

        if w_param == OpCode::Sin || w_param == OpCode::Cos || w_param == OpCode::Tan || w_param == OpCode::Sinh
            || w_param == OpCode::Cosh || w_param == OpCode::Tanh || w_param == OpCode::Sec || w_param == OpCode::Csc
            || w_param == OpCode::Cot || w_param == OpCode::Sech || w_param == OpCode::Csch || w_param == OpCode::Coth {
            if is_current_too_big_for_trig() {
                CURRENT_VAL.store(0, Ordering::SeqCst);
                display_error(calc_display, CALC_E_DOMAIN);
                return;
            }
        }

        CURRENT_VAL.store(sci_calc_functions(CURRENT_VAL.load(Ordering::SeqCst), w_param as u32), Ordering::SeqCst);

        if ERROR.load(Ordering::SeqCst) {
            return;
        }

        display_num(calc_display);

        if w_param == OpCode::Percent {
            check_and_add_last_bin_op_to_history();
            add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
        }

        if INV.load(Ordering::SeqCst) && (w_param == OpCode::Chop || w_param == OpCode::Sin || w_param == OpCode::Cos
            || w_param == OpCode::Tan || w_param == OpCode::Ln || w_param == OpCode::Dms || w_param == OpCode::Degrees
            || w_param == OpCode::Sinh || w_param == OpCode::Cosh || w_param == OpCode::Tanh || w_param == OpCode::Sec
            || w_param == OpCode::Csc || w_param == OpCode::Cot || w_param == OpCode::Sech || w_param == OpCode::Csch
            || w_param == OpCode::Coth) {
            INV.store(false, Ordering::SeqCst);
        }

        return;
    }

    if is_op_in_range(w_param, OpCode::BinEditStart, OpCode::BinEditEnd) {
        if is_bin_op_code(LAST_COM.load(Ordering::SeqCst)) {
            CURRENT_VAL.store(LAST_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
        }

        check_and_add_last_bin_op_to_history();

        if try_toggle_bit(CURRENT_VAL.load(Ordering::SeqCst), w_param as u32 - OpCode::BinEditStart as u32) {
            display_num(calc_display);
        }

        return;
    }

    match w_param {
        OpCode::Clear => {
            if !CHANGE_OP.load(Ordering::SeqCst) {
                check_and_add_last_bin_op_to_history(false);
            }

            LAST_VAL.store(0, Ordering::SeqCst);
            CHANGE_OP.store(false, Ordering::SeqCst);
            OPEN_PAREN_COUNT.store(0, Ordering::SeqCst);
            PRECEDENCE_OP_COUNT.store(0, Ordering::SeqCst);
            TEMP_COM.store(0, Ordering::SeqCst);
            LAST_COM.store(0, Ordering::SeqCst);
            OPCODE.store(0, Ordering::SeqCst);
            PREV_OPCODE.store(0, Ordering::SeqCst);
            NO_PREV_EQU.store(true, Ordering::SeqCst);
            CARRY_BIT.store(0, Ordering::SeqCst);

            calc_display.set_parenthesis_number(0);
            clear_display(calc_display);

            clear_history_line();
            clear_temporary_values(calc_display);
        }
        OpCode::Centr => {
            clear_temporary_values(calc_display);
        }
        OpCode::Back => {
            if RECORD.load(Ordering::SeqCst) {
                INPUT.lock().unwrap().backspace();
                display_num(calc_display);
            } else {
                handle_error_command(w_param);
            }
        }
        OpCode::Equ => {
            while OPEN_PAREN_COUNT.load(Ordering::SeqCst) > 0 {
                if ERROR.load(Ordering::SeqCst) {
                    break;
                }
                TEMP_COM.store(LAST_COM.load(Ordering::SeqCst), Ordering::SeqCst);
                process_command(calc_display, resource_provider, OpCode::CloseP);
                LAST_COM.store(TEMP_COM.load(Ordering::SeqCst), Ordering::SeqCst);
                TEMP_COM.store(w_param as i32, Ordering::SeqCst);
            }

            if !NO_PREV_EQU.load(Ordering::SeqCst) {
                LAST_VAL.store(CURRENT_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
            }

            if is_bin_op_code(LAST_COM.load(Ordering::SeqCst)) {
                CURRENT_VAL.store(LAST_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
            }

            if !opnd_added_to_history() {
                add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
            }

            resolve_highest_precedence_operation(calc_display);
            while PRECEDENCE.load(Ordering::SeqCst) && PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) > 0 {
                PRECEDENCE_OP_COUNT.fetch_sub(1, Ordering::SeqCst);
                OPCODE.store(PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);
                LAST_VAL.store(PRECEDENCE_VALS.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);

                let ni = precedence_of_op(PREV_OPCODE.load(Ordering::SeqCst));
                let nx = precedence_of_op(OPCODE.load(Ordering::SeqCst));
                if ni <= nx {
                    enclose_prec_inversion_brackets();
                }
                pop_last_opnd_start();

                NO_PREV_EQU.store(true, Ordering::SeqCst);

                resolve_highest_precedence_operation(calc_display);
            }

            if !ERROR.load(Ordering::SeqCst) {
                let grouped_string = group_digits_per_radix(NUMBER_STRING.lock().unwrap().clone(), RADIX.load(Ordering::SeqCst));
                complete_equation(grouped_string);
            }

            CHANGE_OP.store(false, Ordering::SeqCst);
            PREV_OPCODE.store(0, Ordering::SeqCst);
        }
        OpCode::OpenP | OpCode::CloseP => {
            if (OPEN_PAREN_COUNT.load(Ordering::SeqCst) >= MAX_PREC_DEPTH && w_param == OpCode::OpenP)
                || (OPEN_PAREN_COUNT.load(Ordering::SeqCst) == 0 && w_param != OpCode::OpenP)
                || (PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) >= MAX_PREC_DEPTH && PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) - 1] != 0) {
                if OPEN_PAREN_COUNT.load(Ordering::SeqCst) == 0 && w_param != OpCode::OpenP {
                    calc_display.on_no_right_paren_added();
                }

                handle_error_command(w_param);
                break;
            }

            if w_param == OpCode::OpenP {
                if is_digit_op_code(LAST_COM.load(Ordering::SeqCst)) || is_unary_op_code(LAST_COM.load(Ordering::SeqCst))
                    || LAST_COM.load(Ordering::SeqCst) == OpCode::Pnt || LAST_COM.load(Ordering::SeqCst) == OpCode::CloseP {
                    process_command(calc_display, resource_provider, OpCode::Mul);
                }

                check_and_add_last_bin_op_to_history();
                add_open_brace_to_history();

                PAREN_VALS.lock().unwrap()[OPEN_PAREN_COUNT.load(Ordering::SeqCst)] = LAST_VAL.load(Ordering::SeqCst);
                OPCODE.lock().unwrap()[OPEN_PAREN_COUNT.load(Ordering::SeqCst)] = if CHANGE_OP.load(Ordering::SeqCst) { OPCODE.load(Ordering::SeqCst) } else { 0 };

                if PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) < PREC_OP.lock().unwrap().len() {
                    PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)] = 0;
                }

                PRECEDENCE_OP_COUNT.fetch_add(1, Ordering::SeqCst);

                LAST_VAL.store(0, Ordering::SeqCst);
                if is_bin_op_code(LAST_COM.load(Ordering::SeqCst)) {
                    CURRENT_VAL.store(0, Ordering::SeqCst);
                }
                TEMP_COM.store(0, Ordering::SeqCst);
                OPCODE.store(0, Ordering::SeqCst);
                CHANGE_OP.store(false, Ordering::SeqCst);
            } else {
                if is_bin_op_code(LAST_COM.load(Ordering::SeqCst)) {
                    CURRENT_VAL.store(LAST_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
                }

                if !opnd_added_to_history() {
                    add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
                }

                CURRENT_VAL.store(do_operation(OPCODE.load(Ordering::SeqCst), CURRENT_VAL.load(Ordering::SeqCst), LAST_VAL.load(Ordering::SeqCst)), Ordering::SeqCst);
                PREV_OPCODE.store(OPCODE.load(Ordering::SeqCst), Ordering::SeqCst);

                while PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) > 0 && PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst) - 1] != 0 {
                    PRECEDENCE_OP_COUNT.fetch_sub(1, Ordering::SeqCst);
                    OPCODE.store(PREC_OP.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);
                    LAST_VAL.store(PRECEDENCE_VALS.lock().unwrap()[PRECEDENCE_OP_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);

                    let ni = precedence_of_op(PREV_OPCODE.load(Ordering::SeqCst));
                    let nx = precedence_of_op(OPCODE.load(Ordering::SeqCst));
                    if ni <= nx {
                        enclose_prec_inversion_brackets();
                    }
                    pop_last_opnd_start();

                    CURRENT_VAL.store(do_operation(OPCODE.load(Ordering::SeqCst), CURRENT_VAL.load(Ordering::SeqCst), LAST_VAL.load(Ordering::SeqCst)), Ordering::SeqCst);
                    PREV_OPCODE.store(OPCODE.load(Ordering::SeqCst), Ordering::SeqCst);
                }

                add_close_brace_to_history();

                OPEN_PAREN_COUNT.fetch_sub(1, Ordering::SeqCst);
                LAST_VAL.store(PAREN_VALS.lock().unwrap()[OPEN_PAREN_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);
                OPCODE.store(OPCODE.lock().unwrap()[OPEN_PAREN_COUNT.load(Ordering::SeqCst)], Ordering::SeqCst);

                CHANGE_OP.store(OPCODE.load(Ordering::SeqCst) != 0, Ordering::SeqCst);
            }

            calc_display.set_parenthesis_number(OPEN_PAREN_COUNT.load(Ordering::SeqCst));

            if !ERROR.load(Ordering::SeqCst) {
                display_num(calc_display);
            }
        }
        OpCode::Hex | OpCode::Dec | OpCode::Oct | OpCode::Bin => {
            set_radix_type_and_num_width(w_param as i32 - OpCode::Hex as i32, -1);
            update_history_expression(RADIX.load(Ordering::SeqCst), PRECISION.load(Ordering::SeqCst));
        }
        OpCode::Qword | OpCode::Dword | OpCode::Word | OpCode::Byte => {
            if RECORD.load(Ordering::SeqCst) {
                CURRENT_VAL.store(INPUT.lock().unwrap().to_rational(RADIX.load(Ordering::SeqCst), PRECISION.load(Ordering::SeqCst)), Ordering::SeqCst);
                RECORD.store(false, Ordering::SeqCst);
            }

            set_radix_type_and_num_width(-1, w_param as i32 - OpCode::Qword as i32);
        }
        OpCode::Deg | OpCode::Rad | OpCode::Grad => {
            ANGLE_TYPE.store(w_param as i32 - OpCode::Deg as i32, Ordering::SeqCst);
        }
        OpCode::Sign => {
            if RECORD.load(Ordering::SeqCst) {
                if INPUT.lock().unwrap().try_toggle_sign(INTEGER_MODE.load(Ordering::SeqCst), get_max_decimal_value_string()) {
                    display_num(calc_display);
                } else {
                    handle_error_command(w_param);
                }
                break;
            }

            if is_bin_op_code(LAST_COM.load(Ordering::SeqCst)) {
                CURRENT_VAL.store(LAST_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
            }

            if !opnd_added_to_history() {
                add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
            }

            CURRENT_VAL.store(-CURRENT_VAL.load(Ordering::SeqCst), Ordering::SeqCst);

            display_num(calc_display);
            add_unary_op_to_history(OpCode::Sign as i32, INV.load(Ordering::SeqCst), ANGLE_TYPE.load(Ordering::SeqCst));
        }
        OpCode::Recall => {
            if SET_CALC_STATE.load(Ordering::SeqCst) {
                SET_CALC_STATE.store(false, Ordering::SeqCst);
            } else {
                CURRENT_VAL.store(*MEMORY_VALUE.lock().unwrap(), Ordering::SeqCst);
            }
            check_and_add_last_bin_op_to_history();
            display_num(calc_display);
        }
        OpCode::MPlus => {
            let result = *MEMORY_VALUE.lock().unwrap() + CURRENT_VAL.load(Ordering::SeqCst);
            *MEMORY_VALUE.lock().unwrap() = truncate_num_for_int_math(result);
        }
        OpCode::MMinus => {
            let result = *MEMORY_VALUE.lock().unwrap() - CURRENT_VAL.load(Ordering::SeqCst);
            *MEMORY_VALUE.lock().unwrap() = truncate_num_for_int_math(result);
        }
        OpCode::Store | OpCode::MClear => {
            *MEMORY_VALUE.lock().unwrap() = if w_param == OpCode::Store { truncate_num_for_int_math(CURRENT_VAL.load(Ordering::SeqCst)) } else { 0 };
        }
        OpCode::Pi => {
            if !INTEGER_MODE.load(Ordering::SeqCst) {
                check_and_add_last_bin_op_to_history();
                CURRENT_VAL.store(if INV.load(Ordering::SeqCst) { TWO_PI } else { PI }, Ordering::SeqCst);
                display_num(calc_display);
                INV.store(false, Ordering::SeqCst);
                break;
            }
            handle_error_command(w_param);
        }
        OpCode::Rand => {
            if !INTEGER_MODE.load(Ordering::SeqCst) {
                check_and_add_last_bin_op_to_history();
                let mut str = String::new();
                write!(str, "{:.precision$}", generate_random_number(), precision = PRECISION.load(Ordering::SeqCst)).unwrap();
                let rat = string_to_rat(false, &str, false, "", RADIX.load(Ordering::SeqCst), PRECISION.load(Ordering::SeqCst));
                CURRENT_VAL.store(if let Some(rat) = rat { rat } else { 0 }, Ordering::SeqCst);
                display_num(calc_display);
                INV.store(false, Ordering::SeqCst);
                break;
            }
            handle_error_command(w_param);
        }
        OpCode::Euler => {
            if !INTEGER_MODE.load(Ordering::SeqCst) {
                check_and_add_last_bin_op_to_history();
                CURRENT_VAL.store(RAT_EXP, Ordering::SeqCst);
                display_num(calc_display);
                INV.store(false, Ordering::SeqCst);
                break;
            }
            handle_error_command(w_param);
        }
        OpCode::Fe => {
            let n_fe = if N_FE.load(Ordering::SeqCst) == NumberFormat::Float { NumberFormat::Scientific } else { NumberFormat::Float };
            N_FE.store(n_fe, Ordering::SeqCst);
            display_num(calc_display);
        }
        OpCode::Exp => {
            if RECORD.load(Ordering::SeqCst) && !INTEGER_MODE.load(Ordering::SeqCst) && INPUT.lock().unwrap().try_begin_exponent() {
                display_num(calc_display);
                break;
            }
            handle_error_command(w_param);
        }
        OpCode::Pnt => {
            if RECORD.load(Ordering::SeqCst) && !INTEGER_MODE.load(Ordering::SeqCst) && INPUT.lock().unwrap().try_add_decimal_pt() {
                display_num(calc_display);
                break;
            }
            handle_error_command(w_param);
        }
        OpCode::Inv => {
            INV.store(!INV.load(Ordering::SeqCst), Ordering::SeqCst);
        }
        _ => {}
    }
}

fn resolve_highest_precedence_operation(calc_display: &dyn CalcDisplay) {
    if OPCODE.load(Ordering::SeqCst) != 0 {
        if NO_PREV_EQU.load(Ordering::SeqCst) {
            HOLD_VAL.store(CURRENT_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
        } else {
            CURRENT_VAL.store(HOLD_VAL.load(Ordering::SeqCst), Ordering::SeqCst);
            display_num(calc_display);
            add_bin_op_to_history(OPCODE.load(Ordering::SeqCst), INTEGER_MODE.load(Ordering::SeqCst));
            add_opnd_to_history(NUMBER_STRING.lock().unwrap().clone(), CURRENT_VAL.load(Ordering::SeqCst));
        }

        CURRENT_VAL.store(do_operation(OPCODE.load(Ordering::SeqCst), CURRENT_VAL.load(Ordering::SeqCst), LAST_VAL.load(Ordering::SeqCst)), Ordering::SeqCst);
        PREV_OPCODE.store(OPCODE.load(Ordering::SeqCst), Ordering::SeqCst);
        LAST_VAL.store(CURRENT_VAL.load(Ordering::SeqCst), Ordering::SeqCst);

        if !ERROR.load(Ordering::SeqCst) {
            display_num(calc_display);
        }

        NO_PREV_EQU.store(false, Ordering::SeqCst);
    } else if !ERROR.load(Ordering::SeqCst) {
        display_num(calc_display);
    }
}

fn check_and_add_last_bin_op_to_history(add_to_history: bool) {
    if CHANGE_OP.load(Ordering::SeqCst) {
        if opnd_added_to_history() {
            remove_last_opnd_from_history();
        }
    } else if opnd_added_to_history() && !ERROR.load(Ordering::SeqCst) {
        if (is_unary_op_code(LAST_COM.load(Ordering::SeqCst)) || LAST_COM.load(Ordering::SeqCst) == OpCode::Sign as i32 || LAST_COM.load(Ordering::SeqCst) == OpCode::CloseP as i32) && OPEN_PAREN_COUNT.load(Ordering::SeqCst) == 0 {
            if add_to_history {
                complete_history_line(group_digits_per_radix(NUMBER_STRING.lock().unwrap().clone(), RADIX.load(Ordering::SeqCst)));
            }
        } else {
            remove_last_opnd_from_history();
        }
    }
}

fn set_primary_display(calc_display: &dyn CalcDisplay, sz_text: &str, is_error: bool) {
    calc_display.set_primary_display(sz_text, is_error);
    calc_display.set_is_in_error(is_error);
}

fn display_announce_binary_operator(calc_display: &dyn CalcDisplay) {
    calc_display.binary_operator_received();
}

fn precedence_of_op(nop_code: i32) -> i32 {
    match nop_code {
        OpCode::Or as i32 | OpCode::Xor as i32 => 0,
        OpCode::And as i32 | OpCode::Nand as i32 | OpCode::Nor as i32 => 1,
        OpCode::Add as i32 | OpCode::Sub as i32 => 2,
        OpCode::Lshf as i32 | OpCode::Rshf as i32 | OpCode::Rshfl as i32 | OpCode::Mod as i32 | OpCode::Div as i32 | OpCode::Mul as i32 => 3,
        OpCode::Pwr as i32 | OpCode::Root as i32 | OpCode::LogBaseY as i32 => 4,
        _ => 0,
    }
}

fn is_gui_setting_op_code(op_code: OpCode) -> bool {
    matches!(op_code, OpCode::Hex | OpCode::Dec | OpCode::Oct | OpCode::Bin | OpCode::Qword | OpCode::Dword | OpCode::Word | OpCode::Byte | OpCode::Deg | OpCode::Rad | OpCode::Grad | OpCode::Inv | OpCode::Fe | OpCode::MClear | OpCode::Back | OpCode::Exp | OpCode::Store | OpCode::MPlus | OpCode::MMinus)
}

fn is_bin_op_code(op_code: OpCode) -> bool {
    matches!(op_code, OpCode::And | OpCode::Or | OpCode::Xor | OpCode::Nand | OpCode::Nor | OpCode::Add | OpCode::Sub | OpCode::Lshf | OpCode::Rshf | OpCode::Rshfl | OpCode::Mod | OpCode::Div | OpCode::Mul | OpCode::Pwr | OpCode::Root | OpCode::LogBaseY)
}

fn is_unary_op_code(op_code: OpCode) -> bool {
    matches!(op_code, OpCode::Chop | OpCode::Sin | OpCode::Cos | OpCode::Tan | OpCode::Ln | OpCode::Dms | OpCode::Degrees | OpCode::Sinh | OpCode::Cosh | OpCode::Tanh | OpCode::Sec | OpCode::Csc | OpCode::Cot | OpCode::Sech | OpCode::Csch | OpCode::Coth | OpCode::Sign)
}

fn is_digit_op_code(op_code: OpCode) -> bool {
    matches!(op_code, OpCode::Digit0 | OpCode::Digit1 | OpCode::Digit2 | OpCode::Digit3 | OpCode::Digit4 | OpCode::Digit5 | OpCode::Digit6 | OpCode::Digit7 | OpCode::Digit8 | OpCode::Digit9 | OpCode::DigitA | OpCode::DigitB | OpCode::DigitC | OpCode::DigitD | OpCode::DigitE | OpCode::DigitF)
}

fn is_op_in_range(op_code: OpCode, start: OpCode, end: OpCode) -> bool {
    (op_code as i32) >= (start as i32) && (op_code as i32) <= (end as i32)
}

fn sci_calc_functions(current_val: Rational, w_param: u32) -> Rational {
    // Implement the scientific calculator functions here
    // This is a placeholder implementation
    current_val
}

fn do_operation(op_code: i32, lhs: Rational, rhs: Rational) -> Rational {
    // Implement the operation logic here
    // This is a placeholder implementation
    lhs + rhs
}

fn display_num(calc_display: &dyn CalcDisplay) {
    // Implement the display logic here
    // This is a placeholder implementation
    calc_display.set_primary_display("0", false);
}

fn check_and_add_last_bin_op_to_history() {
    // Implement the logic to check and add the last binary operator to history
    // This is a placeholder implementation
}

fn add_opnd_to_history(num_str: String, rat: Rational) {
    // Implement the logic to add operand to history
    // This is a placeholder implementation
}

fn change_last_bin_op(op_code: i32, f_prec_inv_to_higher: bool, is_integer_mode: bool) {
    // Implement the logic to change the last binary operator
    // This is a placeholder implementation
}

fn group_digits_per_radix(num_str: String, radix: u32) -> String {
    // Implement the logic to group digits per radix
    // This is a placeholder implementation
    num_str
}

fn complete_equation(grouped_string: String) {
    // Implement the logic to complete the equation
    // This is a placeholder implementation
}

fn remove_last_opnd_from_history() {
    // Implement the logic to remove the last operand from history
    // This is a placeholder implementation
}

fn add_bin_op_to_history(op_code: i32, is_integer_mode: bool) {
    // Implement the logic to add binary operator to history
    // This is a placeholder implementation
}

fn add_unary_op_to_history(op_code: i32, inv: bool, angle_type: i32) {
    // Implement the logic to add unary operator to history
    // This is a placeholder implementation
}

fn is_current_too_big_for_trig() -> bool {
    // Implement the logic to check if the current value is too big for trigonometric functions
    // This is a placeholder implementation
    false
}

fn display_error(calc_display: &dyn CalcDisplay, error_code: u32) {
    // Implement the logic to display error
    // This is a placeholder implementation
    calc_display.set_primary_display("Error", true);
}

fn set_radix_type_and_num_width(radix_type: i32, num_width: i32) {
    // Implement the logic to set radix type and number width
    // This is a placeholder implementation
}

fn update_history_expression(radix: u32, precision: u32) {
    // Implement the logic to update history expression
    // This is a placeholder implementation
}

fn push_last_opnd_start() {
    // Implement the logic to push the last operand start
    // This is a placeholder implementation
}

fn pop_last_opnd_start() {
    // Implement the logic to pop the last operand start
    // This is a placeholder implementation
}

fn enclose_prec_inversion_brackets() {
    // Implement the logic to enclose precedence inversion brackets
    // This is a placeholder implementation
}

fn add_open_brace_to_history() {
    // Implement the logic to add open brace to history
    // This is a placeholder implementation
}

fn add_close_brace_to_history() {
    // Implement the logic to add close brace to history
    // This is a placeholder implementation
}

fn truncate_num_for_int_math(num: Rational) -> Rational {
    // Implement the logic to truncate number for integer math
    // This is a placeholder implementation
    num
}

fn generate_random_number() -> f64 {
    // Implement the logic to generate a random number
    // This is a placeholder implementation
    0.0
}

fn string_to_rat(negative: bool, num_str: &str, exp_negative: bool, exp_str: &str, radix: u32, precision: u32) -> Option<Rational> {
    // Implement the logic to convert string to rational
    // This is a placeholder implementation
    Some(Rational::new(0, 1))
}

fn get_max_decimal_value_string() -> String {
    // Implement the logic to get the maximum decimal value string
    // This is a placeholder implementation
    "0".to_string()
}

fn try_toggle_bit(current_val: Rational, bit: u32) -> bool {
    // Implement the logic to try toggling a bit
    // This is a placeholder implementation
    true
}

fn clear_history_line() {
    // Implement the logic to clear the history line
    // This is a placeholder implementation
}

fn opnd_added_to_history() -> bool {
    // Implement the logic to check if operand is added to history
    // This is a placeholder implementation
    false
}

fn complete_history_line(grouped_string: String) {
    // Implement the logic to complete the history line
    // This is a placeholder implementation
}

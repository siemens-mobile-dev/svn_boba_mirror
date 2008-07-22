
VARARG        MACRO name,tag
        RSEG    CODE:DATA:NOROOT(2)
        PUBLIC  name
name:
        DC32     tag
        ENDM        
        
        VARARG __var_arg_book,0x51
        VARARG __var_arg_htext,0xBE
        VARARG __var_arg_call_back_ok,0x44
        VARARG __var_arg_call_back_prev_act,0xAC
        VARARG __var_arg_call_back_long_back,0x56
        VARARG __var_arg_strinp_mode,0x7F
        VARARG __var_arg_strinp_fixed_text,0xAA
        VARARG __var_arg_strinp_text,0x66
        VARARG __var_arg_strinp_new_line,0x6D
        VARARG __var_arg_strinp_empty_str_en,0x7D
        VARARG __var_arg_strinp_max_len,0x8C
        VARARG __var_arg_strinp_min_len,0x93
        VARARG __var_arg_strinp_is_pass_mode,0xAF
        VARARG __var_arg_strinp_unk1,0x4E
        VARARG __var_arg_strinp_unk2,0xB3
        VARARG __var_arg_strinp_unk3,0x50


        VARARG __var_arg_yesno_pre_question,0x6B
        VARARG __var_arg_yesno_question,0xAD
        VARARG __var_arg_yesno_yes_action,0xC7
        VARARG __var_arg_yesno_no_action,0x9D
        VARARG __var_arg_perinp_row_numbers,0xB7
        VARARG __var_arg_perinp_cur_percent,0x68
        
 
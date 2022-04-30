#ifndef __KITE_DEFS_H__
#define __KITE_DEFS_H__

#include <algorithm>
#include <string>
#include <vector>

// Kite instructions
enum kite_opcode {
    op_nop = 0,
    /* R-type */
    op_add,
    op_and,
    op_div,
    op_divu,
    op_mul,
    op_or,
    op_rem,
    op_remu,
    op_sll,
    op_sra,
    op_srl,
    op_sub,
    op_xor,
    /* I-type */
    op_addi,
    op_andi,
    op_jalr,
    op_slli,
    op_srai,
    op_srli,
    op_lb,
    op_lbu,
    op_lh,
    op_lhu,
    op_lw,
    op_lwu,
    op_ld,
    op_ori,
    op_xori,
    /* S-type */
    op_sb,
    op_sh,
    op_sw,
    op_sd,
    /* SB-type */
    op_beq,
    op_bge,
    op_blt,
    op_bne,
    /* U-type */
    op_lui,
    /* UJ-type */
    op_jal,
    /* FR-type */
    op_faddd,
    op_fdivd,
    op_fmuld,
    op_fsubd,
    /* FI-type */
    op_fld,
    /* FS-type */
    op_fsd,
    num_kite_opcodes,
};

// Kite instruction types
enum kite_opcode_type {
    op_no_type = 0,
    op_r_type,
    op_i_type,
    op_s_type,
    op_sb_type,
    op_u_type,
    op_uj_type,
    op_fr_type,
    op_fi_type,
    op_fs_type,
    num_kite_opcode_types,
};

// Kite registers
enum kite_reg {
    reg_x0 = 0,
    reg_x1,
    reg_x2,
    reg_x3,
    reg_x4,
    reg_x5,
    reg_x6,
    reg_x7,
    reg_x8,
    reg_x9,
    reg_x10,
    reg_x11,
    reg_x12,
    reg_x13,
    reg_x14,
    reg_x15,
    reg_x16,
    reg_x17,
    reg_x18,
    reg_x19,
    reg_x20,
    reg_x21,
    reg_x22,
    reg_x23,
    reg_x24,
    reg_x25,
    reg_x26,
    reg_x27,
    reg_x28,
    reg_x29,
    reg_x30,
    reg_x31,
    reg_f0,
    reg_f1,
    reg_f2,
    reg_f3,
    reg_f4,
    reg_f5,
    reg_f6,
    reg_f7,
    reg_f8,
    reg_f9,
    reg_f10,
    reg_f11,
    reg_f12,
    reg_f13,
    reg_f14,
    reg_f15,
    reg_f16,
    reg_f17,
    reg_f18,
    reg_f19,
    reg_f20,
    reg_f21,
    reg_f22,
    reg_f23,
    reg_f24,
    reg_f25,
    reg_f26,
    reg_f27,
    reg_f28,
    reg_f29,
    reg_f30,
    reg_f31,
    num_kite_regs,
};

// Kite instruction types aligned with the instructions list
static kite_opcode_type kite_op_type[num_kite_opcodes] __attribute__((unused)) = {
    op_no_type, // op_nop
    /* R-type */
    op_r_type,  // op_add
    op_r_type,  // op_and
    op_r_type,  // op_div
    op_r_type,  // op_divu
    op_r_type,  // op_mul
    op_r_type,  // op_or
    op_r_type,  // op_rem
    op_r_type,  // op_remu
    op_r_type,  // op_sll
    op_r_type,  // op_sra
    op_r_type,  // op_srl
    op_r_type,  // op_sub
    op_r_type,  // op_xor
    /* I-type */
    op_i_type,  // op_addi
    op_i_type,  // op_andi
    op_i_type,  // op_jalr
    op_i_type,  // op_slli
    op_i_type,  // op_srai
    op_i_type,  // op_srli
    op_i_type,  // op_lb
    op_i_type,  // op_lbu
    op_i_type,  // op_lh
    op_i_type,  // op_lhu
    op_i_type,  // op_lw
    op_i_type,  // op_lwu
    op_i_type,  // op_ld
    op_i_type,  // op_ori
    op_i_type,  // op_xori
    /* S-type */
    op_s_type,  // op_sb
    op_s_type,  // op_sh
    op_s_type,  // op_sw
    op_s_type,  // op_sd
    /* SB-type */
    op_sb_type, // op_beq
    op_sb_type, // op_bge
    op_sb_type, // op_blt
    op_sb_type, // op_bne
    /* U-type */
    op_u_type,  // op_lui
    /* UJ-type */
    op_uj_type, // op_jal
    /* FR-type */
    op_fr_type, // op_faddd
    op_fr_type, // op_fdivd
    op_fr_type, // op_fmuld
    op_fr_type, // op_fsubd
    /* FI-type */
    op_fi_type, // op_fld
    /* FS-type */
    op_fs_type, // op_fsd
};

// Kite instruction ALU latencies aligned with the instructions list
static unsigned kite_op_latency[num_kite_opcodes] __attribute__((unused)) = {
    1,  // op_nop
    /* R-type */
    1,  // op_add
    1,  // op_and
    2,  // op_div
    2,  // op_divu
    2,  // op_mul
    1,  // op_or
    2,  // op_rem
    2,  // op_remu
    1,  // op_sll
    1,  // op_sra
    1,  // op_srl
    1,  // op_sub
    1,  // op_xor
    /* I-type */
    1,  // op_addi
    1,  // op_andi
    1,  // op_jalr
    1,  // op_slli
    1,  // op_srai
    1,  // op_srli
    1,  // op_lb
    1,  // op_lbu
    1,  // op_lh
    1,  // op_lhu
    1,  // op_lw
    1,  // op_lwu
    1,  // op_ld
    1,  // op_ori
    1,  // op_xori
    /* S-type */
    1,  // op_sb
    1,  // op_sh
    1,  // op_sw
    1,  // op_sd
    /* SB-type */
    1,  // op_beq
    1,  // op_bge
    1,  // op_blt
    1,  // op_bne
    /* U-type */
    1,  // op_lui
    /* UJ-type */
    1,  // op_jal
    /* FR-type */
    1,  // op_faddd
    2,  // op_fdivd
    2,  // op_fmuld
    1,  // op_fsubd
    /* FI-type */
    1,  // op_fld
    /* FS-type */
    1,  // op_fsd
};

// Kite instruction strings aligned with the instructions list
static std::string kite_opcode_str[num_kite_opcodes] __attribute__((unused)) = {
    "nop",
    /* R-type */
    "add",
    "and",
    "div",
    "divu",
    "mul",
    "or",
    "rem",
    "remu",
    "sll",
    "sra",
    "srl",
    "sub",
    "xor",
    /* I-type */
    "addi",
    "andi",
    "jalr",
    "slli",
    "srai",
    "srli",
    "lb",
    "lbu",
    "lh",
    "lhu",
    "lw",
    "lwu",
    "ld",
    "ori",
    "xori",
    /* S-type */
    "sb",
    "sh",
    "sw",
    "sd",
    /* SB-type */
    "beq",
    "bge",
    "blt",
    "bne",
    /* U-type */
    "lui",
    /* UJ-type */
    "jal",
    /* FR-type */
    "fadd.d",
    "fdiv.d",
    "fmul.d",
    "fsub.d",
    /* FI-type */
    "fld",
    /* FS-type */
    "fsd",
};

// Kite register strings aligned with the registers list
static std::string kite_reg_str[num_kite_regs] __attribute__((unused)) = {
    "x0",
    "x1",
    "x2",
    "x3",
    "x4",
    "x5",
    "x6",
    "x7",
    "x8",
    "x9",
    "x10",
    "x11",
    "x12",
    "x13",
    "x14",
    "x15",
    "x16",
    "x17",
    "x18",
    "x19",
    "x20",
    "x21",
    "x22",
    "x23",
    "x24",
    "x25",
    "x26",
    "x27",
    "x28",
    "x29",
    "x30",
    "x31",
    "f0",
    "f1",
    "f2",
    "f3",
    "f4",
    "f5",
    "f6",
    "f7",
    "f8",
    "f9",
    "f10",
    "f11",
    "f12",
    "f13",
    "f14",
    "f15",
    "f16",
    "f17",
    "f18",
    "f19",
    "f20",
    "f21",
    "f22",
    "f23",
    "f24",
    "f25",
    "f26",
    "f27",
    "f28",
    "f29",
    "f30",
    "f31",
};

// Numbers
static std::string numbers = "0123456789";

// Code segment size
#define code_segment_size 1024

// Minimum memory size
#define min_memory_size   2048

// Convert a string to kite_opcode
#define get_opcode(m_string) \
    (kite_opcode)distance(&kite_opcode_str[0], find(&kite_opcode_str[0], &kite_opcode_str[num_kite_opcodes], m_string.c_str()))

// Get the kite_opcode_type of m_op
#define get_op_type(m_op) \
    kite_op_type[m_op]

// Check if kite_opcode_type is a load
#define is_op_load(m_op) \
    (((m_op >= op_lb) && (m_op <= op_ld)) || (m_op == op_fld))

// Check if kite_opcode_type is a store
#define is_op_store(m_op) \
    (((m_op >= op_sb) && (m_op <= op_sd)) || (m_op == op_fsd))

// Get the execution latency of m_op
#define get_op_latency(m_op) \
    kite_op_latency[m_op]

// Convert a string to kite_reg
#define get_regnum(m_string) \
    (kite_reg)distance(&kite_reg_str[0], find(&kite_reg_str[0], &kite_reg_str[num_kite_regs], m_string.c_str()))

// Check if a string is int register
#define is_int_reg(m_regnum) \
    ((m_regnum >= reg_x0) && (m_regnum <= reg_x31))

// Check if a string is fp register
#define is_fp_reg(m_regnum) \
    ((m_regnum >= reg_f0) && (m_regnum <= reg_f31))

// Convert a string to 64-bit integer.
#define get_imm(m_string) \
    (int64_t)strtoll(m_string.c_str(), 0, 10)

// Convert a string to double-precision fp.
#define get_fp(m_string) \
    (double)stod(m_string)

// Check if a string is a positive integer.
#define is_pos_num_str(m_string) \
    (m_string.size() && \
    (m_string.find_first_not_of(numbers) == string::npos))

// Check if a string is a negative integer.
#define is_neg_num_str(m_string) \
    ((m_string[0] == '-') && \
      is_pos_num_str(m_string.substr(1, string::npos)))

// Check if a string is an integer number.
#define is_num_str(m_string) \
    (is_pos_num_str(m_string) || is_neg_num_str(m_string))

// Check if a string is a fp number.
#define is_fp_str(m_string) \
    (is_num_str(m_string.substr(0, m_string.find('.'))) && \
     is_pos_num_str(m_string.substr(m_string.find('.')+1)))

// Check if a string has a valid register format.
#define is_reg_str(m_string) \
    (((m_string[0] == 'x') || (m_string[0] == 'f')) && \
    (m_string.find_first_not_of(numbers, 1) == string::npos))

// Read a register as int.
template <typename T> int64_t read_int(T m_reg) { return *((int64_t*)&m_reg); }

// Read a register as fp.
template <typename T> double read_fp(T m_reg) { return *((double*)&m_reg); }

#endif


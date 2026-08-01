#include "riscv.h"

itype_t detect_type(uint32_t x) {
    switch (x & 0b1111111) {
    case 0b0110011:
        return ITYPE_R;

    case 0b0010111:
        return ITYPE_U;

    case 0b1100011:
        return ITYPE_B;

    case 0b0100011:
        return ITYPE_S;

    case 0b0000011:
    case 0b0010011:
    case 0b1110011: // ECALL opcode
    case 0b0001111: // FENCE opcode
        return ITYPE_I;

    case 0b1101111:
        return ITYPE_J;

    default:
        return ITYPE_NULL;
    }
}

instr_t parse_r_type(uint32_t x) {
    instr_t current;
    current.type = ITYPE_R;
    current.opcode = x & 0b1111111;
    current.rd = x >> 7 & 0b11111;
    current.funct3 = x >> 12 & 0b111;
    current.rs1 = x >> 15 & 0b11111;
    current.rs2 = x >> 20 & 0b11111;
    current.funct7 = x >> 25 & 0b1111111;
    current.imm = NONEXIST;
    return current;
}

instr_t parse_i_type(uint32_t x) {
    uint32_t imm = x >> 20;
    if (imm >> 11)
        imm |= 0xFFFFF800;

    instr_t current = { 0 };
    current.type = ITYPE_I;
    current.opcode = x & 0b1111111;
    current.rd = x >> 7 & 0b11111;
    current.funct3 = x >> 12 & 0b111;
    current.rs1 = x >> 15 & 0b11111;
    current.rs2 = NONEXIST;
    current.funct7 = NONEXIST;
    current.imm = imm;
    return current;
}

instr_t parse_s_type(uint32_t x) {
    uint32_t imm = ((x >> 25 & 0b1111111) << 5) | (x >> 7 & 0b11111);
    if (x >> 31)
        imm |= 0xFFFFF800;

    instr_t current = { 0 };
    current.type = ITYPE_S;
    current.opcode = x & 0b1111111;
    current.rd = NONEXIST;
    current.funct3 = x >> 12 & 0b111;
    current.rs1 = x >> 15 & 0b11111;
    current.rs2 = x >> 20 & 0b11111;
    current.funct7 = NONEXIST;
    current.imm = imm;
    return current;
}

instr_t parse_b_type(uint32_t x) {
    uint32_t imm = 0;
    imm |= (x >> 8 & 0b1111) << 1;
    imm |= (x >> 25 & 0b111111) << 5;
    imm |= (x >> 7 & 0b1) << 11;
    if (x >> 31)
        imm |= 0xFFFFF000;

    instr_t current;
    current.type = ITYPE_B;
    current.opcode = x & 0b1111111;
    current.rd = NONEXIST;
    current.funct3 = x >> 12 & 0b111;
    current.rs1 = x >> 15 & 0b11111;
    current.rs2 = x >> 20 & 0b11111;
    current.funct7 = NONEXIST;
    current.imm = imm;
    return current;
}


instr_t parse_u_type(uint32_t x) {
    uint32_t imm = x >> 12 << 12;

    instr_t current = { 0 };
    current.type = ITYPE_U;
    current.opcode = x & 0b1111111;
    current.rd = x >> 7 & 0b11111;
    current.funct3 = NONEXIST;
    current.rs1 = NONEXIST;
    current.rs2 = NONEXIST;
    current.funct7 = NONEXIST;
    current.imm = imm;
    return current;
}

instr_t parse_j_type(uint32_t x) {
    uint32_t imm = x << 1 >> 1 >> 21 << 1;
    imm |= (x >> 12 & 0b11111111) << 12;
    imm |= (x >> 20 & 1) << 11;
    if (x >> 31)
        imm |= 0xFFF00000;

    instr_t current = { 0 };
    current.type = ITYPE_J;
    current.opcode = x & 0b1111111;
    current.rd = x >> 7 & 0b11111;
    current.funct3 = NONEXIST;
    current.rs1 = NONEXIST;
    current.rs2 = NONEXIST;
    current.funct7 = NONEXIST;
    current.imm = imm;
    return current;
}

instr_t parse_inst(uint32_t x) {
    itype_t tp = detect_type(x);
    switch (tp) {
    case ITYPE_R:
        return parse_r_type(x);
    case ITYPE_I:
        return parse_i_type(x);
    case ITYPE_S:
        return parse_s_type(x);
    case ITYPE_U:
        return parse_u_type(x);
    case ITYPE_B:
        return parse_b_type(x);
    case ITYPE_J:
        return parse_j_type(x);
    default:
        return (instr_t) { 0 };
    }
}

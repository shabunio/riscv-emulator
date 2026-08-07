#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum {
    ITYPE_R,
    ITYPE_I,
    ITYPE_S,
    ITYPE_B,
    ITYPE_U,
    ITYPE_J,
    ITYPE_NULL
} itype_t;

typedef struct {
    itype_t type;
    uint8_t opcode;
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
    uint32_t imm;
} instr_t;

typedef struct {
    uint8_t* mem;
    uint32_t pc;
    uint32_t* regs;
    uint8_t e;
} cpu_t;

#define MEM_SIZE (2 << 16)
#define NONEXIST 0


// prototypes

uint32_t convert_bytes_to_uint32_t(uint8_t* x);
void print_raw_inst(instr_t current);
void print_regs(cpu_t* cpu);
void cpu_halt(cpu_t* cpu);
void cpu_ebreak(cpu_t* cpu);


itype_t detect_type(uint32_t x);
instr_t parse_r_type(uint32_t x);
instr_t parse_i_type(uint32_t x);
instr_t parse_s_type(uint32_t x);
instr_t parse_b_type(uint32_t x);
instr_t parse_j_type(uint32_t x);
instr_t parse_u_type(uint32_t x);
instr_t parse_inst(uint32_t x);


instr_t fetch_inst(cpu_t* cpu);
void cpu_ecall(cpu_t* cpu);
int execute_inst(cpu_t* cpu, instr_t inst);
void cpu_step(cpu_t* cpu);
void cpu_free(cpu_t* cpu);

uint32_t load_image(cpu_t* cpu, const char* filename);


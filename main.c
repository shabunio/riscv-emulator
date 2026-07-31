/*
 *
 *  Single-file RISC-V Emulator
 *  Author:      Dzmitry Shabunio
 *  Year:        2026
 *  License:     MIT
 *  Description: Single-file RISC-V emulator supporting RV32I base
 *               instruction set with full fetch-decode-execute cycle.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


// DEFINE : BEGIN

#define MEM_SIZE (2 << 16)
#define NONEXIST 0

// DEFINE : END


// TYPES : BEGIN

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
    uint8_t halt;
} cpu_t;

// TYPES : END


// UTILS : BEGIN

uint32_t convert_bytes_to_uint32_t(uint8_t* x) {
    return x[0] | x[1] << 8 | x[2] << 16 | x[3] << 24;
}

void print_raw_inst(instr_t current) {
    char c = '?';
    switch (current.type) {
    case ITYPE_B:
        c = 'c'; break;
    case ITYPE_S:
        c = 's'; break;
    case ITYPE_R:
        c = 'r'; break;
    case ITYPE_I:
        c = 'i'; break;
    case ITYPE_U:
        c = 'u'; break;
    case ITYPE_J:
        c = 'j'; break;
    }
    printf("inst(type=%c; ", c);
    printf("op=0x%x; ", current.opcode);
    printf("rd=0x%x; ", current.rd);
    printf("funct3=0x%x; ", current.funct3);
    printf("rs1=0x%x; ", current.rs1);
    printf("rs2=0x%x; ", current.rs2);
    printf("funct7=0x%x; ", current.funct7);
    printf("imm=0x%x)", current.imm);
}

void print_regs(cpu_t* cpu) {
    for (int i = 0; i < 8; i++)
        printf("x%d=%x; ", i, cpu->regs[i]);
    printf("\n");
}

// UTILS : END


// PARSE : BEGIN

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
        return (instr_t){0};
    }
}

// PARSER : END

// LOGIC : BEGIN

instr_t fetch_inst(cpu_t* cpu) {
    return parse_inst(convert_bytes_to_uint32_t((cpu->mem + cpu->pc)));
}

int execute_inst(cpu_t* cpu, instr_t inst) {
    if (inst.type == ITYPE_R) {
        if (inst.funct3 == 0b000) {
            if (inst.funct7 == 0b0000000) {
                // ADD
                cpu->regs[inst.rd] = cpu->regs[inst.rs1] + cpu->regs[inst.rs2];
                cpu->pc += 4;
            }
            else if (inst.funct7 = 0b0100000) {
                // SUB
                cpu->regs[inst.rd] = cpu->regs[inst.rs1] - cpu->regs[inst.rs2];
                cpu->pc += 4;
            }
        }
        else if (inst.funct3 == 0b001) {
            // SLL
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] << cpu->regs[inst.rs2];
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b010) {
            // SLT
            cpu->regs[inst.rd] = (int32_t)cpu->regs[inst.rs1] < (int32_t)cpu->regs[inst.rs2];
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b011) {
            // SLTU
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] < cpu->regs[inst.rs2];
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b100) {
            // XOR
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] ^ cpu->regs[inst.rs2];
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b101) {
            if (inst.funct7 == 0b0000000) {
                // SRL
                cpu->regs[inst.rd] = cpu->regs[inst.rs1] >> cpu->regs[inst.rs2];
                cpu->pc += 4;
            }
            else if (inst.funct7 == 0b0100000) {
                // SRA
                cpu->regs[inst.rd] = (int32_t)cpu->regs[inst.rs1] >> cpu->regs[inst.rs2];
                cpu->pc += 4;
            }
        }
        else if (inst.funct3 == 0b110) {
            // OR
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] | cpu->regs[inst.rs2];
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b111) {
            // AND
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] & cpu->regs[inst.rs2];
            cpu->pc += 4;
        }

    }
    else if (inst.type == ITYPE_I) {
        if (inst.funct3 == 0b000) {
            // ADDI
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] + inst.imm;
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b001) {
            // SLL
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] << (inst.imm & 0b1111);
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b010) {
            // SLTI
            cpu->regs[inst.rd] = (int32_t)cpu->regs[inst.rs1] < (int32_t)inst.imm;
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b011) {
            // SLTIU
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] < inst.imm;
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b101) {
            if (inst.funct7 == 0b0000000) {
                // SRLI
                cpu->regs[inst.rd] = cpu->regs[inst.rs1] >> (inst.imm & 0b1111);
                cpu->pc += 4;
            }
            else if (inst.funct7 == 0b0100000) {
                // SRAI
                cpu->regs[inst.rd] = (int32_t)cpu->regs[inst.rs1] >> (inst.imm & 0b1111);
                cpu->pc += 4;
            }
        }
        else if (inst.funct3 == 0b100) {
            // XORI
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] ^ inst.imm;
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b110) {
            // ORI
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] | inst.imm;
            cpu->pc += 4;
        }
        else if (inst.funct3 == 0b111) {
            // ANDI
            cpu->regs[inst.rd] = cpu->regs[inst.rs1] & inst.imm;
            cpu->pc += 4;
        }
    }
    else if (inst.type == ITYPE_J) {
        //if (inst.funct3 == ) {}
    }
    return 0;
}

void cpu_step(cpu_t* cpu) {
    print_raw_inst(fetch_inst(cpu)); printf("\n");
    cpu->regs[0] = 0; // reset the hard-wired register
    int e = execute_inst(cpu, fetch_inst(cpu));
    if (e) {
        cpu->halt = 1;
    }
}

uint32_t load_image(cpu_t* cpu, const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }
    uint32_t k = 0;
    int c;
    while (k < MEM_SIZE) {
        c = fgetc(fp);
        if (c != EOF) {
            cpu->mem[k] = (uint8_t)c;
            //printf("%x ", cpu->mem[k]);
            k++;
        }
        else {
            break;
        }
    }
    fclose(fp);
    return k;
}

// LOGIC : END


int main(int argc, char* argv[]) {
    cpu_t* cpu = (cpu_t *)malloc(sizeof(cpu_t));
    cpu->mem = (uint8_t *)calloc(MEM_SIZE, sizeof(uint8_t));
    cpu->regs = calloc(32, sizeof(uint32_t));
    cpu->pc = 0;
    cpu->halt = 0;

    char* filename = "samples/prg_slt.bin";
    if (argc > 1) {
        filename = argv[1];
    }

    uint32_t k = load_image(cpu, filename);
    if (k == -1) {
        printf("Image loading failed\n");
        exit(1);
    }

    for (int i = 0; i < (k / 4); i++) {
        cpu_step(cpu);
        print_regs(cpu);
        if (cpu->halt) {
            printf("Halt at pc=0x%x\n", cpu->pc);
            break;
        }
    }
    free(cpu->mem);
    free(cpu->regs);
    free(cpu);
}
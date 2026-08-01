#include "riscv.h"

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
        cpu->regs[inst.rd] = cpu->pc + 4;
        cpu->pc += inst.imm;
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

void cpu_free(cpu_t* cpu) {
    free(cpu->mem);
    free(cpu->regs);
    free(cpu);
}
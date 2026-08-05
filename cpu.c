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
        if (inst.opcode == 0b0010011) {
            if (inst.funct3 == 0b000) {
                // ADDI
                cpu->regs[inst.rd] = cpu->regs[inst.rs1] + inst.imm;
                cpu->pc += 4;
            }
            else if (inst.funct3 == 0b001) {
                // SLL
                cpu->regs[inst.rd] = cpu->regs[inst.rs1] << (inst.imm & 0b11111);
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
                    cpu->regs[inst.rd] = cpu->regs[inst.rs1] >> (inst.imm & 0b11111);
                    cpu->pc += 4;
                }
                else if (inst.funct7 == 0b0100000) {
                    // SRAI
                    cpu->regs[inst.rd] = (int32_t)cpu->regs[inst.rs1] >> (inst.imm & 0b11111);
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
        else if (inst.opcode == 0b1100111) {
            // JALR
            uint32_t addr = (cpu->regs[inst.rs1] + inst.imm) & ~1;
            cpu->regs[inst.rd] = cpu->pc + 4;
            cpu->pc = addr;
            printf("\tjalr: pc=%x\t", cpu->pc);
        }
        else if (inst.opcode == 0b0000011) {
            if (inst.funct3 == 0b000) {
                // LB
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                uint32_t x = cpu->mem[addr];
                if (x >> 7) {
                    x |= ~(uint32_t)0xFF;
                }
                cpu->regs[inst.rd] = x;
                cpu->pc += 4;
            }
            else if (inst.funct3 == 0b001) {
                // LH
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                uint32_t x = cpu->mem[addr] | cpu->mem[addr + 1] << 8;
                if (x >> 15) {
                    x |= ~(uint32_t)0xFFFF;
                }
                cpu->regs[inst.rd] = x;
                cpu->pc += 4;
            }
            else if (inst.funct3 == 0b010) {
                // LW
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                uint32_t x = cpu->mem[addr] | cpu->mem[addr + 1] << 8 | cpu->mem[addr + 2] << 16 | cpu->mem[addr + 3] << 24;
                cpu->regs[inst.rd] = x;
                cpu->pc += 4;
            }
            else if (inst.funct3 == 0b100) {
                // LBU
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                uint32_t x = cpu->mem[addr];
                cpu->regs[inst.rd] = x;
                cpu->pc += 4;
            }
            else if (inst.funct3 == 0b101) {
                // LHU
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                uint32_t x = cpu->mem[addr] | cpu->mem[addr + 1] << 8;
                cpu->regs[inst.rd] = x;
                cpu->pc += 4;
            }
        }
    }
    else if (inst.type == ITYPE_J) {
        // JAL
        cpu->regs[inst.rd] = cpu->pc + 4;
        cpu->pc += inst.imm;
    }
    else if (inst.type == ITYPE_B) {
        if (inst.opcode == 0b1100011) {
            if (inst.funct3 == 0b000) {
                // BEQ
                if (cpu->regs[inst.rs1] == cpu->regs[inst.rs2]) {
                    cpu->pc = (inst.imm + cpu->pc) & ~1;
                }
            }
            else if (inst.funct3 == 0b001) {
                // BNE
                if (cpu->regs[inst.rs1] != cpu->regs[inst.rs2]) {
                    cpu->pc = (inst.imm + cpu->pc) & ~1;
                }
            }
            else if (inst.funct3 == 0b100) {
                // BLT
                if ((int32_t)cpu->regs[inst.rs1] < (int32_t)cpu->regs[inst.rs2]) {
                    cpu->pc = (inst.imm + cpu->pc) & ~1;
                }
            }
            else if (inst.funct3 == 0b101) {
                // BGE
                if ((int32_t)cpu->regs[inst.rs1] > (int32_t)cpu->regs[inst.rs2]) {
                    cpu->pc = (inst.imm + cpu->pc) & ~1;
                }
            }
            else if (inst.funct3 == 0b110) {
                // BLTU
                if (cpu->regs[inst.rs1] < cpu->regs[inst.rs2]) {
                    cpu->pc = (inst.imm + cpu->pc) & ~1;
                }
            }
            else if (inst.funct3 == 0b111) {
                // BGEU
                if (cpu->regs[inst.rs1] > cpu->regs[inst.rs2]) {
                    cpu->pc = (inst.imm + cpu->pc) & ~1;
                }
            }
        }
    }
    else if (inst.type == ITYPE_U) {
        if (inst.opcode == 0b0110111) {
            // LUI
            cpu->regs[inst.rd] = inst.imm;
        }
        else if (inst.opcode == 0b0010111) {
            // AUIPC
            cpu->regs[inst.rd] = inst.imm + cpu->pc;
        }
    }
    else if (inst.type == ITYPE_S) {
        if (inst.opcode == 0b0100011) {
            if (inst.funct3 == 0b000) {
                // SB
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                cpu->mem[addr] = cpu->regs[inst.rs2] & 0xFF;
                cpu->pc += 4;
            }
            else if (inst.funct3 == 0b001) {
                // SH
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                cpu->mem[addr] = cpu->regs[inst.rs2] & 0xFF;
                cpu->mem[addr + 1] = cpu->regs[inst.rs2] >> 8 & 0xFF;
                cpu->pc += 4;
            }
            else if (inst.funct3 == 0b010) {
                // SW
                uint32_t addr = cpu->regs[inst.rs1] + inst.imm;
                cpu->mem[addr] = cpu->regs[inst.rs2] & 0xFF;
                cpu->mem[addr + 1] = cpu->regs[inst.rs2] >> 8 & 0xFF;
                cpu->mem[addr + 2] = cpu->regs[inst.rs2] >> 16 & 0xFF;
                cpu->mem[addr + 3] = cpu->regs[inst.rs2] >> 24 & 0xFF;
                cpu->pc += 4;
            }
        }
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
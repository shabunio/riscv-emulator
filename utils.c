#include "riscv.h"

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

void cpu_halt(cpu_t* cpu) {
    printf("CPU halted at PC=0x%x\n", cpu->pc);
    cpu_free(cpu);
	exit(1);
}
#include "riscv.h"

void cpu_ebreak(cpu_t* cpu) {
    printf("EBREAK triggered\n");
    cpu->pc += 4;
}

void cpu_ecall(cpu_t* cpu) {
    printf("ECALL triggered\n");
    cpu->pc += 4;
}
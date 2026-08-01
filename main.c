/*
 *
 *  Single-file RISC-V Emulator
 *  Author:      Dzmitry Shabunio
 *  Year:        2026
 *  License:     MIT
 *  Description: RISC-V emulator supporting RV32I base
 *               instruction set with full fetch-decode-execute cycle.
 *
 */

#include "riscv.h"

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
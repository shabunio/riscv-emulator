/*
 *
 *  RISC-V Emulator
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
    cpu->mem = calloc(MEM_SIZE, sizeof(uint8_t));
    if (cpu->mem == NULL) {
        printf("Memory allocation failed\n");
        free(cpu);
        exit(1);
    }
    cpu->regs = calloc(32, sizeof(uint32_t));
    cpu->pc = 0;
    cpu->e = 0;
    char* filename = "binary.dat";
    if (argc > 1) {
        filename = argv[1];
    }

    long k = load_image(cpu, filename);
    if (k == -1) {
        printf("Image loading failed\n");
        exit(1);
    }

    for (int i = 0; i < (k/4); i++) {
        cpu_step(cpu);
        if (cpu->e == 1) {
			cpu_halt(cpu);
            break;
        }
    }
    uint8_t e = cpu->e;
    print_dec_regs(cpu);
    print_hex_regs(cpu);
    cpu_free(cpu);
    exit(e);
}
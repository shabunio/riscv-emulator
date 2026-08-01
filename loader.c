#include "riscv.h"

uint32_t load_image(cpu_t* cpu, const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }
    uint32_t k;
    k = fread(cpu->mem, sizeof(uint8_t), MEM_SIZE, fp);
    fclose(fp);
    return k;
}
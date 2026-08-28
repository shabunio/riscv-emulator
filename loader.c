#include "riscv.h"

long load_image(cpu_t* cpu, const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }
    long k;
    k = fread(cpu->mem, sizeof(uint8_t), MEM_SIZE, fp);
    fclose(fp);
    return k;
}
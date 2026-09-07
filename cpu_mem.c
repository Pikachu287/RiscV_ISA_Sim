#include "RV32I.h"

Memory * create_memory(__uint32_t size, __uint32_t base_adress){
    Memory * mem = malloc(sizeof(Memory));
    mem->data = calloc(size, 1); //Zero initialised data
    mem->base = base_adress;
}
void destroy_memory(Memory * mem){
    free(mem);
}

CPU *create_CPU(Memory *mem){
    CPU *cpu = calloc(1,sizeof(CPU)); //calloc cause zero initialised X.
    cpu->mem = mem;
    cpu->PC = 0;
    cpu->running = 1;
}
void destroy_CPU(CPU *cpu){
    free(cpu);
}


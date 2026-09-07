#include "RV32I.h"


Memory * create_memory(UINT32_T size, UINT32_T base_adress){
    Memory * mem = malloc(sizeof(Memory));
    mem->data = calloc(size, 1); //Zero initialised data
    mem->base = base_adress;
    printf("Memory initialized with size: %d bytes - (%d KB) - (%d MB)\n", size, size / 1000, size / 1000000);
    return mem;
}
void destroy_memory(Memory * mem){
    free(mem);
}
 
CPU *create_CPU(Memory *mem){
    CPU *cpu = calloc(1,sizeof(CPU)); //calloc cause zero initialised X.
    cpu->mem = mem;
    cpu->PC = 0;
    cpu->running = 1;
    printf("CPU initialized\n");
    return cpu;
}
void destroy_CPU(CPU *cpu){
    free(cpu);
}

void execute(CPU * cpu, Instruction * inst){
    
}






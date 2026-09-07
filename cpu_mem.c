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

void save_byte(Memory * mem, UINT32_T adress, UINT8_T val){
    if (adress >mem->size) printf("Illegal adress\n"); return;
    memcpy(mem->data[adress],&val,sizeof(val));
}

void save_half(Memory * mem, UINT32_T adress, UINT16_T val){
    if ((adress+1) >mem->size) printf("Illegal adress\n"); return;
    memcpy(mem->data[adress],&val,sizeof(val));
}

void save_word(Memory * mem, UINT32_T adress, UINT32_T val){
    if ((adress+3) >mem->size) printf("Illegal adress\n"); return;
    memcpy(mem->data[adress],&val,sizeof(val));
}

SINT8_T load_byte(Memory * mem, UINT32_T adress){
    if (adress >mem->size) printf("Illegal adress\n"); return;
    return mem->data[adress];
}

SINT16_T load_half(Memory * mem, UINT32_T adress){
    if (adress+1 >mem->size) printf("Illegal adress\n"); return;
    return (mem->data[adress] << 8) | (mem->data[adress+1]);
}

SINT32_T load_word(Memory * mem, UINT32_T adress){
    if (adress+3 >mem->size) printf("Illegal adress\n"); return;
    return ((UINT32_T)mem->data[adress+3] << 24) | ((UINT32_T)mem->data[adress+2] << 16) | ((UINT32_T)mem->data[adress+1] << 8) | ((UINT32_T)mem->data[adress]);
}


void execute_R(CPU * cpu, Instruction * inst);
void execute_I(CPU * cpu, Instruction * inst);
void execute_L(CPU * cpu, Instruction * inst);
void execute_ECALL(CPU * cpu, Instruction * inst);
void execute_JALR(CPU * cpu, Instruction * inst);
void execute_JAL(CPU * cpu, Instruction * inst);
void execute_S(CPU * cpu, Instruction * inst);
void execute_B(CPU * cpu, Instruction * inst);
void execute_LUI(CPU * cpu, Instruction * inst);
void execute_AUIPC(CPU * cpu, Instruction * inst);
void execute_FENCE(CPU * cpu, Instruction * inst);




void execute(CPU * cpu, Instruction * inst){
    
}






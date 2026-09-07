#include "RV32I.h"
#include "cpu_mem.c"
#include "instruction.c"

int main(){
    Memory * mem = create_memory(1024 * 1024, 0x0);
    CPU * cpu = create_CPU(mem);
    Instruction inst1 = decode_Instruction(0x00200093);
    // printf("Hex: %#010x\n",inst1.raw);
    // print_type(inst1.type);
    return 0;
}
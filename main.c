#include "RV32I.h"
#include "cpu_mem.c"

int main(){
    Memory * mem = create_memory(1024 * 1024, 0x0);
    CPU * cpu = create_CPU(mem);
    
    return 0;
}
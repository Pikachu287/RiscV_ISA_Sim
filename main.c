#include "RV32I.h"
#include "cpu_mem.c"
#include "instruction.c"

int main(){
    UINT32_T inst_list = { 
        0x003100B3,//R-type
        0x403100B3,
        0x003110B3,
        0x003120B3,
        0x003130B3,
        0x003140B3,
        0x003150B3,
        0x403150B3,
        0x003160B3,
        0x003170B3,//Last R-type
        0x00A10093,//I-type
        0xFFF00293, //Extra addi for neg value
        0x00A12093,
        0x00A13093,
        0x00A14093,
        0x00A16093,
        0x00A17093,//Last I arith
        0x00311093, //I-type shift
        0x00315093,
        0x40315093,//Last I shift
        0x00410083, //Loads
        0x00411083,
        0x00412083,
        0x00414083,
        0x00415083,//Last load
        0x00310223, //S-type
        0x00311223,
        0x00312223, //Last S-type
        0x123450B7, //U-type - lui
        0x12345097 //auipc
    };


    Memory * mem = create_memory(1024, 0x0);
    CPU * cpu = create_CPU(mem);
    Instruction inst1 = decode_Instruction(0x00200093);
    save_byte(mem, 0x10, 0x12);
    save_half(mem, 0x20, 0x1234);
    save_word(mem, 0x30, 0x12345678);
    // printf("All mem:",mem->size);
    for (int i = 0; i < mem->size-100;i++){
        if (mem->data[i] != 0){
            printf("Mem_Byte: %#04x\t has\t%#02x\n",i,mem->data[i]);
        }
    }
    printf("CPU REG x%d has value %#08x\n",5,cpu->X[5]);
    cpu->X[5] = load_word(cpu->mem,0x30);
    printf("CPU REG x%d has value %#08x\n",5,cpu->X[5]);



    printf("Done printing changed bytes in mem and checking for little endian\n");
    save_byte(mem, 0x50, 0x66);
    save_byte(mem, 0x51, 0x75);
    save_byte(mem, 0x52, 0x63);
    save_byte(mem, 0x53, 0x6B);
    cpu->X[11] = 0x50; // manual add adress and print_string to reg.
    cpu->X[10] = 4;
    printf("Entering manual syscall;\n");
    execute_syscall(cpu,cpu->X[10],cpu->X[11]);
    printf("Done syscall\n");
    int wait;
    printf("Waiting (any input will stop program)");
    scanf("%d",&wait);
    

    return 0;
}
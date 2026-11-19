#include "RV32I.h"
#include "cpu_mem.c"
#include "instruction.c"
// #include <X11/Xlib.h> // For future window addition perhaps???

int main(){
    // UINT32_T inst_list = { 
    //     0x003100B3,//R-type
    //     0x403100B3,
    //     0x003110B3,
    //     0x003120B3,
    //     0x003130B3,
    //     0x003140B3,
    //     0x003150B3,
    //     0x403150B3,
    //     0x003160B3,
    //     0x003170B3,//Last R-type
    //     0x00A10093,//I-type
    //     0xFFF00293, //Extra addi for neg value
    //     0x00A12093,
    //     0x00A13093,
    //     0x00A14093,
    //     0x00A16093,
    //     0x00A17093,//Last I arith
    //     0x00311093, //I-type shift
    //     0x00315093,
    //     0x40315093,//Last I shift
    //     0x00410083, //Loads
    //     0x00411083,
    //     0x00412083,
    //     0x00414083,
    //     0x00415083,//Last load
    //     0x00310223, //S-type
    //     0x00311223,
    //     0x00312223, //Last S-type
    //     0x123450B7, //U-type - lui
    //     0x12345097 //auipc
    // };

    // /*
    // Test saving and loading bytes
    // Test ecall
    
    // */
    // Memory * mem = create_memory(1024);
    // CPU * cpu = create_CPU(mem, NULL);
    // Instruction inst1 = decode_Instruction(0x00200093);
    // save_byte(mem, 0x10, 0x12);
    // save_half(mem, 0x20, 0x1234);
    // save_word(mem, 0x30, 0x12345678);
    // // printf("All mem:",mem->size);
    // for (int i = 0; i < mem->size-100;i++){
    //     if (mem->data[i] != 0){
    //         printf("Mem_Byte: %#04x\t has\t%#02x\n",i,mem->data[i]);
    //     }
    // }
    // printf("CPU REG x%d has value %#08x\n",5,cpu->X[5]);
    // cpu->X[5] = load_word(cpu->mem,0x30);
    // printf("CPU REG x%d has value %#08x\n",5,cpu->X[5]);



    // printf("Done printing changed bytes in mem and checking for little endian\n");
    // save_byte(mem, 0x50, 0x66);//f
    // save_byte(mem, 0x51, 0x75);//u
    // save_byte(mem, 0x52, 0x63);//c
    // save_byte(mem, 0x53, 0x6B);//k - Nothing is stored at next reg so it is already a null terminator
    // cpu->X[11] = 0x50; // manual add adress and print_string to reg.
    // cpu->X[10] = 4;
    // printf("Entering manual syscall;\n");
    // execute_syscall(cpu,cpu->X[10],cpu->X[11]);
    // printf("Done syscall\n");
    // int wait;
    // printf("Waiting (any input will stop program)");
    // scanf("%d",&wait);
    // printf("Deleting CPU and making new.\n");
    // destroy_CPU(cpu);
    // destroy_memory(mem);
    // printf("Creating brand new CPU and memory for running .bin files\n");

    
    //Make loop for getting instructions here
    FILE *instr_f;
    instr_f = fopen("bin_files/string.bin", "rb");
    if (instr_f == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    fseek(instr_f, 0, SEEK_END);
    int instr_len = ftell(instr_f);
    rewind(instr_f);
    printf("Length: %d\n",instr_len);

    FILE *out;
    out  = fopen("Output_new.txt","w");
    if (out == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }


    fprintf(out, "Dump of all instructions and register values\n");
    fprintf(out, "INST\trd, rs1/2/imm\n");
    fprintf(out, "________________________________________________________________\n");


    Memory * mem = create_memory(1024 * 1024);
    CPU * cpu = create_CPU(mem, out);
    Instruction inst;
    UINT8_T data_raw;
    //Copy all .bin from file to cpu memory.
    if (fread(mem->data, 1, instr_len, instr_f) != (size_t)instr_len) {
        perror("Can't read binary");
        return 1;
    }

    UINT32_T instr_raw;
    while(cpu->running){
        if (cpu->PC + 3 >= mem->size) {// PC ran off the end of memory
            printf("PC out of range: %#x\n", cpu->PC);
            break;
        }   
        instr_raw = load_word(cpu->mem,cpu->PC);
        inst = decode_Instruction(instr_raw);
        execute(cpu, &inst);
    }
    fprintf(cpu->output, "________________________________________________________________\n");
    fprintf(cpu->output, "Register dump:\tHex\t\tDecimal\t\tChar\n");
    for (int i = 0; i < 32; i++){
        printf("Reg: %d\t: %#8x\t%#10d\t%#5c\n", i, cpu->X[i], cpu->X[i], cpu->X[i]);
        fprintf(cpu->output,"Reg: %d\t: %#8x\t%#10d\t%#5c\n", i, cpu->X[i], cpu->X[i], cpu->X[i]);
    }
    //Dump 1kb of memory to file
    fprintf(cpu->output, "________________________________________________________________\n");
    fprintf(cpu->output, "Dumb of the lowest 1024 Bytes of memory\n");
    for (int i = 0; i < 1024;i++){
        fprintf(cpu->output,"Byte:\t%#5d\tStores:\t%d\n",i,cpu->mem[i]);
    }


    //When done totally enter here.
    destroy_memory(mem);
    destroy_CPU(cpu);
    return 0;
}

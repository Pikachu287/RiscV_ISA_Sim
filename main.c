#include "RV32I.h"
#include "cpu_mem.c"
#include "instruction.c"
// #include <X11/Xlib.h> // For future window addition perhaps???

int main(){
    //Get instructions.
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
    //Loop that fetches instruction, decodes and executes each loop until cpu->running = 0
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
        printf("Reg: %d\t: %#8x\t%#10d\t%#5c\n", i, cpu->X[i], cpu->X[i], cpu->X[i]);// The #10 and #5 is undefined behavior (Which just adds spaces, so it doesn't break.)
        fprintf(cpu->output,"Reg: %d\t: %#8x\t%#10d\t%#5c\n", i, cpu->X[i], cpu->X[i], cpu->X[i]);
    }
    //Dump 1kb of cpu memory to file
    fprintf(cpu->output, "________________________________________________________________\n");
    fprintf(cpu->output, "Dumb of the lowest 1024 Bytes of memory\n");
    for (int i = 0; i < 1024;i++){
        fprintf(cpu->output,"Byte:\t%#5d\tStores:\t%d\n",i,cpu->mem[i]);
    }
    //dump all registers in a .res file
    FILE * dump_out;
    dump_out = fopen("dumb.res","wb");

    if (dump_out == NULL){
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    if (fwrite(cpu->X, sizeof(UINT32_T), 32, dump_out) != 32){
        perror("Error writing register dump");
        fclose(dump_out);
        return EXIT_FAILURE;
    }
    fclose(dump_out);

    //When done totally enter here.
    destroy_memory(mem);
    destroy_CPU(cpu);
    return 0;
}

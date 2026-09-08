#include "RV32I.h"


Memory * create_memory(UINT32_T size, UINT32_T base_adress){
    Memory * mem = malloc(sizeof(Memory));
    mem->data = calloc(size, 1); //Zero initialised data
    mem->base = base_adress;
    mem ->size = size;
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
    memcpy(&mem->data[adress],&val,1);
}

void save_half(Memory * mem, UINT32_T adress, UINT16_T val){
    if ((adress+1) >mem->size) printf("Illegal adress\n"); return;
    memcpy(&mem->data[adress],&val,2);
}

void save_word(Memory * mem, UINT32_T adress, UINT32_T val){
    if ((adress+3) >mem->size) printf("Illegal adress\n"); return;
    memcpy(&mem->data[adress],&val,4);
}

SINT8_T load_byte(Memory * mem, UINT32_T adress){
    if (adress >mem->size) printf("Illegal adress\n"); return -1;
    return mem->data[adress];
}

SINT16_T load_half(Memory * mem, UINT32_T adress){
    if (adress+1 >mem->size) printf("Illegal adress\n"); return -1;
    return (mem->data[adress] << 8) | (mem->data[adress+1]);
}

SINT32_T load_word(Memory * mem, UINT32_T adress){
    if (adress+3 >mem->size) printf("Illegal adress\n"); return -1;
    return ((UINT32_T)mem->data[adress+3] << 24) | ((UINT32_T)mem->data[adress+2] << 16) | ((UINT32_T)mem->data[adress+1] << 8) | ((UINT32_T)mem->data[adress]);
}


void execute_R(CPU * cpu, Instruction * inst){
    switch(inst->funct3){
        case 0x0: //ADD or SUB
            switch(inst->funct7){
                case 0://ADD
                    printf("add x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
                    cpu->X[inst->rd] = cpu->X[inst->rs1] + cpu->X[inst->rs2];
                    break;
                case 0x20://SUB
                    printf("add x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
                    cpu->X[inst->rd] = cpu->X[inst->rs1] - cpu->X[inst->rs2];
                    break;
            }
            break;
        case 0x1://SLL
            printf("sll x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
            cpu->X[inst->rd] = cpu->X[inst->rs1] << (cpu->X[inst->rs2] & 0x1F);
            break;
        case 0x2://SLT - set Less Than signed
            printf("slt x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
            cpu->X[inst->rd] = (cpu->X[inst->rs1] <(SINT32_T)cpu->X[inst->rs2]) ? 1 : 0;
            break;
        case 0x3: //SLTU - set Less Than unsigned
            printf("sltu x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
            cpu->X[inst->rd] = (cpu->X[inst->rs1] < cpu->X[inst->rs2]) ? 1 : 0;
            break;
        case 0x4://XOR
            printf("xor x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
            cpu->X[inst->rd] = cpu->X[inst->rs1] ^ cpu->X[inst->rs2];
            break;
        case 0x5://SRA and SRL
            switch(inst->funct7){
                case 0://SRA
                    printf("sra x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
                    cpu->X[inst->rd] = sign_Extend(cpu->X[inst->rs1] >> (cpu->X[inst->rs2] & 0x1F),31-(cpu->X[inst->rs2] & 0x1F));
                    break;
                case 0x20://SRL
                    printf("srl x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
                    cpu->X[inst->rd] = cpu->X[inst->rs1] >> (cpu->X[inst->rs2] & 0x1F);
                    break;
            }
            break;
        case 0x6://OR
            printf("or x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
            cpu->X[inst->rd] = cpu->X[inst->rs1] | cpu->X[inst->rs2];
            break;
        case 0x7://AND
            printf("and x%d, x%d, x%d\n",inst->rd,inst->rs1,inst->rs2);
            cpu->X[inst->rd] = cpu->X[inst->rs1] & cpu->X[inst->rs2];
            break;
        default:
            printf("Unknown R Instruction\tfunct3:%d\n",inst->funct3);
            break;

    }
}

void execute_I(CPU * cpu, Instruction * inst){

}

void execute_L(CPU * cpu, Instruction * inst){

}

void execute_ECALL(CPU * cpu, Instruction * inst){

}

void execute_JALR(CPU * cpu, Instruction * inst){

}

void execute_JAL(CPU * cpu, Instruction * inst){

}

void execute_S(CPU * cpu, Instruction * inst){

}

void execute_B(CPU * cpu, Instruction * inst){

}

void execute_LUI(CPU * cpu, Instruction * inst){

}

void execute_AUIPC(CPU * cpu, Instruction * inst){

}

void execute_FENCE(CPU * cpu, Instruction * inst){

}





void execute(CPU * cpu, Instruction * inst){
    
}






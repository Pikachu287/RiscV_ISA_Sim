#include "RV32I.h"


Memory * create_memory(UINT32_T size, UINT32_T base_adress){
    Memory * mem = malloc(sizeof(Memory));
    mem->data = calloc(size, 1); //Zero initialised data
    mem->base = base_adress;
    mem ->size = size;
    
    
    printf("Memory initialized with size: %d bytes - (%d KB) - (%d MB)\n", size, size / 1000, size / 1000000);
    printf("Memory adress base: 0x%#08x\n",mem->base);
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
    if (adress > mem->size) {
        printf("Illegal adress\n");
        return;
    }
    //Memcpy doesn't check for little endian but uses the standard pc way (most of the time little endian)
    memcpy(&mem->data[adress + mem->base],&val,1);
}

void save_half(Memory * mem, UINT32_T adress, UINT16_T val){
    if ((adress+1) > mem->size) {
        printf("Illegal adress\n");
        return;
    }
    //Memcpy doesn't check for little endian but uses the standard pc way (most of the time little endian)
    memcpy(&mem->data[adress + mem->base],&val,2);
}

void save_word(Memory * mem, UINT32_T adress, UINT32_T val){
    if ((adress+3) > mem->size) {
        printf("Illegal adress\n");
        return;
    }
    //Memcpy doesn't check for little endian but uses the standard pc way (most of the time little endian)
    memcpy(&mem->data[adress + mem->base],&val,4);
}

UINT8_T load_byte(Memory * mem, UINT32_T adress, UINT8_T use_extend){
    //Little endian
    if (adress > mem->size) {
        printf("Illegal adress\n");
        return 0;
    }
    if (use_extend){
        return sign_Extend(mem->data[adress],7);
    }
    return mem->data[adress],7;
}

UINT16_T load_half(Memory * mem, UINT32_T adress, UINT8_T use_extend){
    //Little endian
    if ((adress+1) > mem->size) {
        printf("Illegal adress\n");
        return 0;
    }
    if (use_extend){
        return sign_Extend((mem->data[adress] << 8) | (mem->data[adress+1]),15);
    }
    return (mem->data[adress] << 8) | (mem->data[adress+1]);
}

UINT32_T load_word(Memory * mem, UINT32_T adress){
    if ((adress+3) > mem->size) {
        printf("Illegal adress\n");
        return 0;
    }
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
           printf("UNKNOWN R funct3 code %#02x\n",inst->funct3);
            break;

    }
}

void execute_I(CPU * cpu, Instruction * inst){
    SINT32_T shamt = inst->imm & 0x1F;
    switch(inst->funct3){
        case 0x0://ADDI
            printf("addi x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
            cpu->X[inst->rd] = cpu->X[inst->rs1] + inst->imm;
            break;
        case 0x1://SLLI
            printf("slli x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
            cpu->X[inst->rd] = cpu->X[inst->rs1] << shamt;
            break;
        case 0x2://SLTI
            printf("slti x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
            cpu->X[inst->rd] = ((SINT32_T)cpu->X[inst->rs1] < inst->imm) ? 1 : 0;
            break;
        case 0x3://SLTIU
            printf("sltiu x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
            cpu->X[inst->rd] = (cpu->X[inst->rs1] < inst->imm) ? 1 : 0;
            break;
        case 0x4://XORI
            printf("xori x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
            cpu->X[inst->rd] = cpu->X[inst->rs1] ^ inst->imm;
            break;
        case 0x5://SRLI eller SRAI
            switch(inst->funct7){
                case 0x0: //SRLI - fill upper with 0
                    printf("srli x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
                    cpu->X[inst->rd] = cpu->X[inst->rs1] >> shamt;
                    break;
                case 0x20://SRAI - fill upper with copt of sign bit
                    printf("srai x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
                    cpu->X[inst->rd] = sign_Extend(cpu->X[inst->rs1] >> shamt,31-shamt);
                    break;
            }
            break;
        case 0x6://ORI
            printf("ori x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
            cpu->X[inst->rd] = cpu->X[inst->rs1] | inst->imm;
            break;
        case 0x7://ANDI
            printf("andi x%d, x%d, %d\n",inst->rd,inst->rs1,inst->imm);
            cpu->X[inst->rd] = cpu->X[inst->rs1] & inst->imm;
            break;
        default:
            printf("UNKNOWN I funct3 code %#02x\n",inst->funct3);
            break;
    }

}

void execute_L(CPU * cpu, Instruction * inst){
    UINT32_T address = cpu->X[inst->rs1] + inst->imm; //Always just calculate the adress
    switch (inst->funct3){
    case 0x0://lb
        printf("lb x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        cpu->X[inst->rd] = load_byte(cpu->mem,address,TRUE);
        break;
    case 0x1://lh
        printf("lh x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        cpu->X[inst->rd] = load_half(cpu->mem,address,TRUE);
        break;
    case 0x2://lw
        printf("lw x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        cpu->X[inst->rd] = load_word(cpu->mem,address);
        break;
    case 0x4://lbu
        printf("lbu x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        cpu->X[inst->rd] = load_byte(cpu->mem,address,FALSE);
        break;
    case 0x5://lhu
        printf("lhu x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        cpu->X[inst->rd] = load_half(cpu->mem,address,FALSE);
        break;
    default:
        printf("UNKNOWN L funct3 code %#02x\n",inst->funct3);
        break;
    }
}

void execute_ECALL(CPU * cpu, Instruction * inst){
    if (inst->imm == 0x001){//Check if bit 0 (of imm) is set for ebreak instead of ecall
        printf("EBREAK\n");
        cpu->running = 0;
        return;
    }
    printf("ECALL %d\n", cpu->X[17]);//Check a7 for syscall/ecall variable
    cpu->running = 0;
}

void execute_JALR(CPU * cpu, Instruction * inst){
    printf("jalr x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
    cpu->X[inst->rd] = cpu->PC + 4;
    cpu->PC = (cpu->X[inst->rs1] + inst->imm) & ~0x1;
}

void execute_JAL(CPU * cpu, Instruction * inst){
    printf("jal x%d, %d\n",inst->rd,inst->imm);
    cpu->X[inst->rd] = cpu->PC + 4;
    cpu->PC = cpu->PC + inst->imm;
}

void execute_S(CPU * cpu, Instruction * inst){
    UINT32_T adress = cpu->X[inst->rs1] + inst->imm;
    switch (inst->funct3){
    case 0x0://sb
        printf("sb x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        save_byte(cpu->mem,adress,cpu->X[inst->rs2]);
        break;
    case 0x1://sh
        printf("sh x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        save_half(cpu->mem,adress,cpu->X[inst->rs2]);
        break;
    case 0x2://sw
        printf("sw x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
        save_word(cpu->mem,adress,cpu->X[inst->rs2]);
        break;
    default:
        printf("UNKNOWN S funct3 code %#02x\n",inst->funct3);
        break;
    }
}

void execute_B(CPU * cpu, Instruction * inst){
    switch (inst->funct3){
    case 0x0://beq - branch when rs1==rs2
        printf("beq x%d, x%d, %d\n",inst->rs1,inst->rs2,inst->imm);
        cpu->PC = (cpu->X[inst->rs1] == cpu->X[inst->rs2]) ? cpu->PC + inst->imm : cpu->PC + 4;
        break;
    case 0x1://bne - branch when rs1!=rs2
        printf("bne x%d, x%d, %d\n",inst->rs1,inst->rs2,inst->imm);
        cpu->PC = (cpu->X[inst->rs1] != cpu->X[inst->rs2]) ? cpu->PC + inst->imm : cpu->PC + 4;
        break;
    case 0x4://blt - branch when rs1 < rs2 signed
        printf("blt x%d, x%d, %d\n",inst->rs1,inst->rs2,inst->imm);
        cpu->PC = ((SINT32_T)cpu->X[inst->rs1] < (SINT32_T)cpu->X[inst->rs2]) ? cpu->PC + inst->imm : cpu->PC + 4;
        break;
    case 0x5://bge - branch when rs1 >= rs2 signed
        printf("bge x%d, x%d, %d\n",inst->rs1,inst->rs2,inst->imm);
        cpu->PC = ((SINT32_T)cpu->X[inst->rs1] >= (SINT32_T)cpu->X[inst->rs2]) ? cpu->PC + inst->imm : cpu->PC + 4;
        break;
    case 0x6://bltu - branch when rs1 < rs2 unsigned
        printf("bltu x%d, x%d, %d\n",inst->rs1,inst->rs2,inst->imm);
        cpu->PC = (cpu->X[inst->rs1] < cpu->X[inst->rs2]) ? cpu->PC + inst->imm : cpu->PC + 4;
        break;
    case 0x7://bgeu - branch when rs1 >= rs2 unsigned
        printf("bgeu x%d, x%d, %d\n",inst->rs1,inst->rs2,inst->imm);
        cpu->PC = (cpu->X[inst->rs1] >= cpu->X[inst->rs2]) ? cpu->PC + inst->imm : cpu->PC + 4;
        break;
    default:
        printf("UNKNOWN B funct3 code %#02x\n",inst->funct3);
        break;
    }
}

void execute_LUI(CPU * cpu, Instruction * inst){
    printf("lui x%d, %#07x\n",inst->rd,inst->imm);
    cpu->X[inst->rd] = inst->imm;
}

void execute_AUIPC(CPU * cpu, Instruction * inst){
    printf("auipc x%d, %#07x\n",inst->rd,inst->imm);
    cpu->X[inst->rd] = cpu->PC + inst->imm;
}

void execute_FENCE(CPU * cpu, Instruction * inst){
    printf("FENCE not implemented\n");
}





void execute(CPU * cpu, Instruction * inst){
    switch(inst->type){
        case R_Type:
            execute_R(cpu, inst);
        case I_Type:
            execute_I(cpu, inst);
        case L_Type:
            execute_L(cpu, inst);
        case ECALL:
            execute_ECALL(cpu, inst);
        case JALR:
            execute_JALR(cpu, inst);
        case JAL:
            execute_JAL(cpu, inst);
        case S_Type:
            execute_S(cpu, inst);
        case B_Type:
            execute_B(cpu, inst);
        case LUI:
            execute_LUI(cpu, inst);
        case AUIPC:
            execute_AUIPC(cpu, inst);
        case FENCE:
            execute_FENCE(cpu, inst);
        case UNKNOWN:
            printf("UNKOWN OPCODE\n");
            
        default:
            break;
    }
    //Always makes reg ZERO have value 0, even if it was overwritten by accident.
    cpu->X[0] = 0;
}






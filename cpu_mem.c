#include "RV32I.h"

/// @brief Creates a new Memory with the given parameters. The base of the memory is initialized to be at the top of the stack.
/// @param size The size of the memory in bytes when it is initialized.
/// @return Memory pointer to the newly initialized memory.
Memory * create_memory(UINT32_T size){
    Memory * mem = malloc(sizeof(Memory));
    mem->data = calloc(size, 1); //Zero initialised data
    mem->base = size - 1; // Top of the stack
    mem ->size = size; //Size of the memory
    
    printf("Memory initialized with size: %d bytes - (%d KB) - (%d MB)\n", size, size / 1000, size / 1000000);
    printf("Memory adress base: %#08x\n",mem->base);
    return mem;
}

/// @brief Free the memory
/// @param mem The memory pointer of the memory that will be free.
void destroy_memory(Memory * mem){
    free(mem);
}

/// @brief Creates a new cpu and initializes sp(x2) to the base of the memory that was given.
/// All other registers for the CPU is initialized to 0.
/// @param mem The memory for which the cpu is to be initialed with.
/// @return Returns a pointer to the CPU struct.
CPU *create_CPU(Memory *mem){
    CPU *cpu = calloc(1,sizeof(CPU)); //calloc cause zero initialised X.
    cpu->mem = mem;
    cpu->PC = 0;
    cpu->X[2] = cpu->mem->base;
    cpu->running = 1;
    printf("CPU initialized\n");
    return cpu;
}

/// @brief Destroys a CPU(Not it's memory)
/// @param cpu The CPU that is to be destroyed.
void destroy_CPU(CPU *cpu){
    free(cpu);
}

/// @brief Save one byte on the memory using memcpy.
/// @param mem The memory where the data is saved to.
/// @param adress The adress in memory where the data is saved.
/// @param val The data to be saved.
void save_byte(Memory * mem, UINT32_T adress, UINT8_T val){
    if (adress > mem->size) {
        printf("Illegal adress\n");
        return;
    }
    //Memcpy doesn't check for little endian but uses the standard pc way (most of the time little endian)
    memcpy(&mem->data[adress],&val,1);
}

/// @brief Save half word on the memory using memcpy.
/// @param mem The memory where the data is saved to.
/// @param adress The adress in memory where the data is saved.
/// @param val The data to be saved.
void save_half(Memory * mem, UINT32_T adress, UINT16_T val){
    if ((adress+1) > mem->size) {
        printf("Illegal adress\n");
        return;
    }
    //Memcpy doesn't check for little endian but uses the standard pc way (most of the time little endian)
    memcpy(&mem->data[adress],&val,2);
}

/// @brief Save word on the memory using memcpy. 
/// @param mem The memory where the data is saved to.
/// @param adress The adress in memory where the data is saved.
/// @param val The data to be saved.
void save_word(Memory * mem, UINT32_T adress, UINT32_T val){
    if ((adress+3) > mem->size) {
        printf("Illegal adress\n");
        return;
    }
    //Memcpy doesn't check for little endian but uses the standard pc way (most of the time little endian)
    memcpy(&mem->data[adress],&val,4);
}

/// @brief Loads a single byte from memory
/// @param mem The memory from which the data is loaded from
/// @param adress The adress of the data that is loaded.
/// @param use_extend If the load is signed, then do a sign-extension.
/// @return Returns the sign_extended or unsigned version of the data.
UINT8_T load_byte(Memory * mem, UINT32_T adress, UINT8_T use_extend){
    //Little endian
    if (adress > mem->size) {
        printf("Illegal adress\n");
        return 0;
    }
    if (use_extend){
        return sign_Extend(mem->data[adress],7);
    }
    return mem->data[adress];
}

/// @brief Loads 2 bytes from memory
/// @param mem The memory from which the data is loaded from
/// @param adress The adress of the data that is loaded.
/// @param use_extend If the load is signed, then do a sign-extension.
/// @return Returns the sign_extended or unsigned version of the data. The data is loaded in little endian fashion.
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

/// @brief Loads word from memory
/// @param mem The memory from which the data is loaded from
/// @param adress The adress of the data that is loaded.
/// @param use_extend If the load is signed, then do a sign-extension.
/// @return Returns the sign_extended or unsigned version of the data. The data is loaded in little endian fashion.
UINT32_T load_word(Memory * mem, UINT32_T adress){
    if ((adress+3) > mem->size) {
        printf("Illegal adress\n");
        return 0;
    }
    return ((UINT32_T)mem->data[adress+3] << 24) | ((UINT32_T)mem->data[adress+2] << 16) | ((UINT32_T)mem->data[adress+1] << 8) | ((UINT32_T)mem->data[adress]);
}

/// @brief Executes an R-type instruction based on which funct3 and funct7 value the instruction has.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
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
    //R-type is linear (pc+4)
    cpu->PC = cpu->PC + 4;
}

/// @brief Executes an I-type instruction based on which funct3 and funct7 value the instruction has.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
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
    //I-type is linear (pc+4)
    cpu->PC = cpu->PC + 4;
}

/// @brief Executes a load-type(I-type actually but has another OPCODE) instruction based on which funct3 value the instruction has.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
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
    //Load: I-type is linear (pc+4)
    cpu->PC = cpu->PC + 4;
}

/// @brief Executes a ecall/ebreak(where it just stops) instruction based on which imm value the instruction has.  
/// When ecall is called, this function will execute a syscall, based on the value of a0 and a1.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
void execute_ECALL(CPU * cpu, Instruction * inst){
    //Add 4 to PC even for an ecall or ebreak
    cpu->PC = cpu->PC + 4;

    if (inst->imm == 0x001){//Check if bit 0 (of imm) is set for ebreak instead of ecall
        printf("EBREAK\n");
        cpu->running = 0;
        return;
    }
    printf("ECALL %d\n", cpu->X[10]);//Check a0 for syscall/ecall variable in a0.

    //since the cpu is passed to the function. the variables a0 and a1 are redundant since these are stores in CPU but i cant bother to change it.
    execute_syscall(cpu,cpu->X[10],cpu->X[11]);
}

/// @brief Execute function for JALR instruction.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
void execute_JALR(CPU * cpu, Instruction * inst){
    printf("jalr x%d, %d(x%d)\n",inst->rd,inst->imm,inst->rs1);
    cpu->X[inst->rd] = cpu->PC + 4;
    cpu->PC = (cpu->X[inst->rs1] + inst->imm) & ~0x1;
}

/// @brief Execute function for JAL instruction.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
void execute_JAL(CPU * cpu, Instruction * inst){
    printf("jal x%d, %d\n",inst->rd,inst->imm);
    cpu->X[inst->rd] = cpu->PC + 4;
    cpu->PC = cpu->PC + inst->imm;
}

/// @brief Execute function for S-type instructions.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
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
    //S-type is linear (pc+4)
    cpu->PC = cpu->PC + 4;
}

/// @brief Execute function for B-type instructions.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
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
        //if unknown B funct3, then pc + 4
        cpu->PC = cpu->PC + 4;
        break;
    }
}

/// @brief Execute function for LUI instruction.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
void execute_LUI(CPU * cpu, Instruction * inst){
    printf("lui x%d, %#07x\n",inst->rd,inst->imm);
    cpu->X[inst->rd] = inst->imm;
    //LUI is linear (pc+4)
    cpu->PC = cpu->PC + 4;
}

/// @brief Execute function for AUIPC instruction.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
void execute_AUIPC(CPU * cpu, Instruction * inst){
    printf("auipc x%d, %#07x\n",inst->rd,inst->imm);
    cpu->X[inst->rd] = cpu->PC + inst->imm;
    //AUIPC is linear (pc+4) even though it uses pc.
    cpu->PC = cpu->PC + 4;
}

/// @brief Execute function for FENCE instruction. This however does nothing but add 4 to the PC counter.
/// @param cpu The CPU which the instruction is executed on.
/// @param inst The instruction that is to be executed.
void execute_FENCE(CPU * cpu, Instruction * inst){
    printf("FENCE not implemented\n");
    //Don't know the fuck what FENCE is, but just skip that shiiii.
    cpu->PC = cpu->PC + 4;
}

/// @brief The main execute function which chooses which type of function/instruction to execute, based on the type of the instruction.
/// @param cpu The CPU on which the instruction is executed on.
/// @param inst The instruction that is executed.
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
            cpu->PC = cpu->PC + 4;
        default:
            break;
    }
    //Always makes reg ZERO have value 0, even if it was overwritten by accident.
    cpu->X[0] = 0;
}

/// @brief Syscall function which acts as the response from an ecall.
/// @param cpu The cpu on which the syscall is executed on.
/// @param a0 Variable for which syscall that is to be executed. Redundant since CPU is passed
/// @param a1 Variable for some syscalls. Redundant since CPU is passed
void execute_syscall(CPU * cpu, UINT32_T a0, UINT32_T a1){
    UINT8_T temp;
    UINT32_T i = 0;
    switch(a0){
        case SYS_print_int:
            printf("%d\n",a1);
            break;
        case SYS_print_string:
            do {//Perhabs add check?
                temp = load_byte(cpu->mem,a1 + i, FALSE);
                if (temp == 0x0 || (a1+i >= cpu->mem->size)){ //Hit null terminator or uninitialized memory.
                    break;
                }
                printf("%c",temp);
                i += 1;
                
            }while(i < MAX_STRING_LENGTH);
            printf("\n");
            break;
        case SYS_sbrk:
            printf("SYS_sbrk do somethingxxxxxxxxXXXX:):):):):):):):) IDK what to do with this:(\n");
            break;
        case SYS_exit:
            cpu->running = 0;
            printf("exit\n");
            break;
        case SYS_print_character:
            printf("%c\n",(char)a1);
            break;
        case SYS_exit2:
            cpu->running = 0;
            printf("exit %d\n", a1);
            break;
        default:
            printf("WTF WRONG ECALL VARIABLE??? - Couldn't be me that made a mistake.\\");
            break;
    }
}




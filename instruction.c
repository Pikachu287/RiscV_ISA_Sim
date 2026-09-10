#include "RV32I.h"

/// @brief Extends an int number of a size_bit to 32bits using sign extension.
/// @param n number
/// @param extend_bit Extends the integer n from this bit forward. Zero indexed.
/// @return Returns the 32bit sign_extended version of an int of size size_bit
SINT32_T sign_Extend(SINT32_T n, SINT32_T extend_bit){
    if (extend_bit >= 31){
        printf("Error, cant extend more than 32bits\n");
        return -1;
    }

    UINT32_T no_extend = (n & (1 << (extend_bit))) ? 0 : 1;
    if(no_extend){
        return n;
    }
    //Creates an int with all bits at 1 except from extend_bit LSB
    UINT32_T temp = 0xFFFFFFFF;
    temp = temp << extend_bit;
    return (SINT32_T)(temp | n);
    
}

Inst_type decode_Opcode(UINT8_T opcode){
    Inst_type ins_type;
    switch(opcode){
        case OPC_R:
            ins_type = R_Type;
            break;
        case OPC_I:
            ins_type = I_Type;
            break;            
        case OPC_L:
            ins_type = L_Type;
            break;
        case OPC_ECALL:
            ins_type = ECALL;
            break;
        case OPC_JALR:
            ins_type = JALR;
            break;
        case OPC_JAL:
            ins_type = JAL;
            break;
        case OPC_S:
            ins_type = S_Type;
            break;
        case OPC_B:
            ins_type = B_Type;
            break;
        case OPC_LUI:
            ins_type = LUI;
            break;
        case OPC_AUIPC:
            ins_type = AUIPC;
            break;
        case OPC_FENCE:
            ins_type = FENCE;
            break;
        default:
            ins_type = UNKNOWN;
            break;

    }
    return ins_type;
}

Instruction decode_Instruction(UINT32_T raw_instruction){
    Instruction inst;
    //Assignment for all values - no matter the type
    inst.raw = raw_instruction;
    inst.opcode = raw_instruction & 0x7F;
    inst.funct3 = (raw_instruction >> 12) & 0x7;
    inst.rd = (raw_instruction >> 7) & 0x1F;
    inst.rs1 = (raw_instruction >> 15) & 0x1F;
    inst.rs2 = (raw_instruction >> 20) & 0x1F;
    inst.funct7 = (raw_instruction >> 25) & 0x7F;
    inst.type = decode_Opcode(inst.opcode);
    inst.imm = 0;
    //Specific imm based on the instruction type.
    SINT32_T temp;
    switch(inst.type){
        case R_Type://NO IMM USED
            break;
        case I_Type:
            inst.imm = sign_Extend((raw_instruction >> 20) & 0xFFF,11); // 12bit MSB - sign extended
            break;
        case L_Type:
            inst.imm = sign_Extend((raw_instruction >> 20) & 0xFFF,11);
            break;
        case ECALL://NO IMM USED
            inst.imm = raw_instruction>>20;
            break;
        case JALR:
            inst.imm = sign_Extend((raw_instruction >> 20) & 0xFFF,11);
            break;
        case JAL:
            temp = (((raw_instruction >> 31) & 0x1) << 20) | 
            (((raw_instruction >> 12) & 0xFF) << 12) | 
            (((raw_instruction >> 20) & 0x1) << 11) | 
            (((raw_instruction >> 21) & 0x3FF) << 1);
            inst.imm = sign_Extend(temp,20);
            break;
        case S_Type:
            inst.imm = sign_Extend((inst.funct7 << 5) | inst.rd,11);
            break;
        case B_Type:
            temp = (((raw_instruction >> 31) & 0x1) << 12) | 
            (((raw_instruction >> 7) & 0x1) << 11) | 
            (((raw_instruction >> 25) & 0x3F) << 5) | 
            (((raw_instruction >> 8) & 0xF) << 1);
            inst.imm = sign_Extend(temp,12);
            break;
        case LUI:
            inst.imm = ((raw_instruction >> 12) & 0xFFFFF) << 12;
            break;
        case AUIPC:
            inst.imm = ((raw_instruction >> 12) & 0xFFFFF) << 12;
            break;
        case FENCE://NO IMM USED
            break;
        default: 
            break;
        
    }
    return inst;
}

void print_type(Inst_type type){
    switch(type){
        case R_Type:
            printf("R_Type");
            break;
        case I_Type:
            printf("I_Type");
            break;
        case L_Type:
            printf("L_Type");
            break;
        case ECALL:
            printf("ECALL");
            break;
        case JALR:
            printf("JALR");
            break;
        case JAL:
            printf("JAL");
            break;
        case S_Type:
            printf("S_Type");
            break;
        case B_Type:
            printf("B_Type");
            break;
        case LUI:
            printf("LUI");
            break;
        case AUIPC:
            printf("AUIPC");
            break;
        case FENCE:
            printf("FENCE");
            break;
        default: 
            printf("UNKNOWN TYPE");
            break;
        
    }
}





#include <stdio.h>
#include <stdlib.h>

/// @brief Extends an int number of a size_bit to 32bits using sign extension.
/// @param n number
/// @param size_bit how many bits the number currently is
/// @return Returns the 32bit sign_extended version of an int of size size_bit

int sign_Extend(int n, int size_bit){
    if (size_bit >= 32){
        printf("Error, cant extend more than 32bits\n");
    }
    int no_extend = (n & (1 << (size_bit-1))) ? 0 : 1;
    if(no_extend){
        return n;
    }else{
        //Creates an int with all bits at 1 except from size_bit LSB
        int temp = 0xFFFFFFFF;
        temp = temp << size_bit;
        return temp | n;
    }
}


int main(){
    int X[32] = {};
    int instructions[] = {
        0x00200093, // addi x1 x0 2
        0x002081b3, // add x3 x1 x2
        0x00300113, // addi x2 x0 3
        0x00811213, // slli x4 x2 8
        0x00125213, // srli x4 x4 1
        0x00202023, // sw x2, 0(x0)
        0x00002503, // lw x10, 0(x0)
        0x004000a3, // sb x4, 1(x0)
        0x00102583 // lw x11, 1(x0)
    };
    
    //MEM alloc
    int *mem = (int *) malloc(1024*1024); //1MB
    if (mem == NULL){
        printf("Memory allocation failed\n");
        return 1;
    }
    int pc = 0;
    
    
    
    while (1){
        int instr = instructions[pc >> 2];

        //uni
        int opcode = instr & 0x7F;
        int funct3 = (instr >> 12) & 0x7;
        int rd = (instr >> 7) & 0x1F;
        
        
        int rs1 = (instr >> 15) & 0x1F;
        int rs2 = (instr >> 20) & 0x1F;
        int funct7 = (instr >> 25) & 0x7F;

        int imm = (instr >> 20) & 0xFFF; // 12bit MSB
        int shamt = imm & 0x1F; //For shifting only uses 5-LSB of IMM value to shift
        
        int imm_S = (funct7 << 5) | rd; //12bit imm for type-S/B instructions
            

        switch (opcode)
        {
        case 0x33://R-type opcode=b0110011

            switch(funct3){
            case 0x0: //add or sub
                switch(funct7){
                    case 0: //add
                        printf("add\n");
                        X[rd] = X[rs1] + X[rs2];
                        break;
                    case 0x20: //sub
                        X[rd] = X[rs1] - X[rs2];
                        break;
                    }
                break;
            case 0x1: //sll
                X[rd] = X[rs1] << X[rs2];
                break;
            case 0x2: //slt - set Less Than signed
                X[rd] = (X[rs1] < X[rs2]) ? 1 : 0;
                break;
            case 0x3: //sltu - set Less Than unsigned
                X[rd] = (X[rs1] < X[rs2]) ? 1 : 0;
                break;
            case 0x4: //XOR
                X[rd] = X[rs1] ^ X[rs2];
                break;
            case 0x5: //SRA and SRL
                switch(funct7){
                    case 0: //SRL
                        X[rd] = X[rs1] >> X[rs2];
                        break;
                    case 0x20: //SRA
                        X[rd] = X[rs1] >> X[rs2];
                        break;
                }
                break;
            case 0x6: //OR
                X[rd] = X[rs1] | X[rs2];
                break;
            case 0x7: //AND
                X[rd] = X[rs1] & X[rs2];
                break;
            }
            pc += 4;
            break;

        case 0x13://I-type opcode=b0010011
            switch(funct3){
            case 0x0: //addi
                printf("addi\n");
                X[rd] = X[rs1] + imm;
                break;
            case 0x1: //slli
                X[rd] = X[rs1] << shamt;
                break;
            case 0x2://slti
                X[rd] = (X[rs1] < imm) ? 1 : 0; 
                break;
            case 0x3://sltiu - LOOK IF IT NEEDS TO BE CHANGED FOR UNSIGN
                X[rd] = (X[rs1] < imm) ? 1 : 0; 
                break;
            case 0x4://xori
                X[rd] = X[rs1] ^ imm;
                break;
            case 0x5://srli / srai
                switch(funct7){
                    case 0x0://srli
                        X[rd] = X[rs1] >> shamt;
                        break;
                    case 0x20://srai
                        X[rd] = X[rs1] >> shamt;
                        break;
                }
                break;
            case 0x6://ori
                X[rd] = X[rs1] | imm;
                break;
            case 0x7://andi
                X[rd] = X[rs1] & imm;
                break;
            }
            pc += 4;
            break;
        
        case 0x3://I-type opcode=b0000011
            switch(funct3){
            case 0x0: //lb
                //Sign extend only first byte of data from memory,
                X[rd] = sign_Extend((mem[X[rs1] + sign_Extend(imm,12)]) & 0xFF,8);
                break;
            case 0x1://lh
                //Sign extend second byte of data from memory,
                X[rd] = sign_Extend((mem[X[rs1] + sign_Extend(imm,12)]) & 0xFFFF,16);
                break;
            case 0x2://lw
                printf("LW: %d\n",mem[X[rs1] + sign_Extend(imm,12)]);
                X[rd] = mem[X[rs1] + sign_Extend(imm,12)];
                break;
            case 0x4://lbu
                //Extract only byte
                X[rd] = mem[X[rs1] + sign_Extend(imm,12)] & 0xFF;
                break;
            case 0x5://lhu
                //Extract 2 bytes
                X[rd] = mem[X[rs1] + sign_Extend(imm,12)] & 0xFFFF;
                break;

            }
            pc += 4;
            break;
        
        case 0x73: //I-type opcode=b1110011 - ecall, ebreak
            pc += 4;
            break;
        
        case 0x67://I-type opcode=b1100111 -jalr
            pc += 4; //TEMPORARY THIS IS WRONG
            break;
        
        case 0x6F://UJ-type opcode=b1101111 - jal
            //jal - Jump and Link, rd = pc + 4; 
            //pc += imm
            pc += 4; //TEMPORARY THIS IS WRONG
            break;
        
        case 0x23://S-type opcode=b0100011
            switch(funct3){
            case 0x0: //sb
                mem[rs1 + sign_Extend(imm_S,12)] = X[rs2] & 0xFF;
                break;
            case 0x1: //sh
                mem[rs1 + sign_Extend(imm_S,12)] = X[rs2] & 0xFFFF;
                break;
            case 0x2: //sw
                mem[rs1 + sign_Extend(imm_S,12)] = X[rs2];
                break;
            }
            pc += 4;
            break;
        
        case 0x63://SB-type opcode=b1100011
            switch(funct3){
            case 0x0: //beq rs1==rs2
                pc = (X[rs1] == X[rs2]) ? pc + imm_S : pc + 4;
                break;
            case 0x1: //bne rs1!=rs2
                pc = (X[rs1] != X[rs2]) ? pc + imm_S : pc + 4;
                break;
            case 0x4: //blt rs1 < rs2 signed
                pc = (X[rs1] < X[rs2]) ? pc + imm_S : pc + 4;
                break;
            case 0x5: //bge rs1 >= rs2 signed
                pc = (X[rs1] >= X[rs2]) ? pc + imm_S : pc + 4;
                break;
            case 0x6: // bltu rs1 < rs2 unsigned
                pc = (X[rs1] < X[rs2]) ? pc + imm_S : pc + 4;
                break;
            case 0x7: //bgeu rs1 >= rs2 unsigned
                pc = (X[rs1] >= X[rs2]) ? pc + imm_S : pc + 4;
                break;
            }
            break;
        
        case 0x37: //U-type opcode=b0110111 - lui
            // lui - load upper immediate rd = imm << 12
            pc += 4;
            break;
        case 0x17: //U-type opcode=b0010111 - auipc
            // auipc - Add upper immediate to pc. rd = pc + (imm << 12)
            pc += 4;
            break;
        
        

        case 0xF: //I-type opcode=b0001111 - fence - not required
            pc += 4;
            break;
        
        
        default:
            break;
        }
        
        if (pc >= sizeof(instructions)) break;
    }
    //prints all values of all registers.
    for (int i = 0; i < sizeof(X)/sizeof(X[0]);i++){
        printf("Reg: x%d stores: %d\n",i,X[i]);
    }

    printf("done");
    return 0;
}
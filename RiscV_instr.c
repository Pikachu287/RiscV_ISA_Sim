#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/// @brief Extends an int number of a size_bit to 32bits using sign extension.
/// @param n number
/// @param size_bit how many bits the number currently is
/// @return Returns the 32bit sign_extended version of an int of size size_bit
int sign_Extend(int n, int size_bit){
    if (size_bit >= 32){
        printf("Error, cant extend more than 32bits\n");
        return -1;
    }
    int no_extend = (n & (1 << (size_bit))) ? 0 : 1;
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
    //Initialize all 32 reg to 0
    int X[32] = {};
    // int instructions[] = {
    //     0x00200093, // addi x1 x0 2
    //     0x002081b3, // add x3 x1 x2
    //     0x00300113, // addi x2 x0 3
    //     0x00811213, // slli x4 x2 8
    //     0x00125213, // srli x4 x4 1
    //     0x00202023, // sw x2, 0(x0)
    //     0x00002503, // lw x10, 0(x0)
    //     0x004000a3, // sb x4, 1(x0)
    //     0x00102583 // lw x11, 1(x0)
    // };
    
    //MEM alloc zero initialized
    unsigned char *mem = (unsigned char *) calloc(1024*1024, 1); //1MB
    if (mem == NULL){
        perror("Memory allocation failed");
        return 1;
    }
    int pc = 0;
    //Directory
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
    } else {
       perror("getcwd() error");
    }
    //File opening
    FILE *instr_f;
    
    // fptr = fopen("C:\\Users\\chris\\OneDrive - Danmarks Tekniske Universitet\\Bachelor\\3 Semester\\ComputerArkitektur\\RiscV_ISA_Sim\\instructions.bin", "rb");
    
    instr_f = fopen("bin_files\\loop.bin", "rb");
    if (instr_f == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    fseek(instr_f, 0, SEEK_END);
    int instr_len = ftell(instr_f);
    printf("Length of instructions in bytes: %d\n",instr_len);
    FILE *out;
    out  = fopen("Output.txt","w");
    if (out == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    fprintf(out, "Dump of all instructions and register values\n");
    fprintf(out, "INST\trd, rs1/2/imm\n");
    fprintf(out, "________________________________________________________________\n");
    
    int instr;
    int running = 1;
    while (running){
        
        
        if (fseek(instr_f, (long) pc, SEEK_SET) != 0) {
            perror("End of file reached");
            break;
        }
        if (fread(&instr,4,1,instr_f) != 1){
            perror("Can't read instruction");
            break;
        }
        // printf("PC: %d instruction %#010x Decimal: %d\n",pc,instr,instr);
        

        //uni / Default placement of part instructions
        int opcode = instr & 0x7F;
        int funct3 = (instr >> 12) & 0x7;
        int rd = (instr >> 7) & 0x1F;
        int rs1 = (instr >> 15) & 0x1F;
        int rs2 = (instr >> 20) & 0x1F;
        int funct7 = (instr >> 25) & 0x7F;

        //Imm value, default to I-type imm but is changed to fit other instructions
        int imm = (instr >> 20) & 0xFFF; // 12bit MSB
        //Addres value
        int addr = 0;
        
        //Large switch statement firstly with OPCODE, then funct3 then funct7 or IMM values for some instructions.
        switch (opcode)
        {
        case 0x33://R-type opcode=b0110011

            switch(funct3){
            case 0x0: //add or sub
                switch(funct7){
                    case 0: //add
                        printf("add x%d, x%d, x%d\n",rd,rs1,rs2);
                        fprintf(out,"add \tx%d, x%d, x%d\n",rd,rs1,rs2);
                        X[rd] = X[rs1] + X[rs2];
                        break;
                    case 0x20: //sub
                        printf("sub x%d, x%d, x%d\n",rd,rs1,rs2);
                        fprintf(out,"sub \tx%d, x%d, x%d\n",rd,rs1,rs2);
                        X[rd] = X[rs1] - X[rs2];
                        break;
                    }
                break;
            case 0x1: //sll
                printf("sll x%d, x%d, x%d\n",rd,rs1,rs2);
                fprintf(out,"sll \tx%d, x%d, x%d\n",rd,rs1,rs2);
                X[rd] = X[rs1] << (X[rs2] & 0x1F);
                break;
            case 0x2: //slt - set Less Than signed
                printf("slt x%d, x%d, x%d\n",rd,rs1,rs2);
                fprintf(out,"slt \tx%d, x%d, x%d\n",rd,rs1,rs2);
                X[rd] = (X[rs1] < X[rs2]) ? 1 : 0;
                break;
            case 0x3: //sltu - set Less Than unsigned
                printf("sltu x%d, x%d, x%d\n",rd,rs1,rs2);
                fprintf(out,"sltu \tx%d, x%d, x%d\n",rd,rs1,rs2);
                X[rd] = ((unsigned int)X[rs1] < (unsigned int)X[rs2]) ? 1 : 0;
                break;
            case 0x4: //XOR
                printf("xor x%d, x%d, x%d\n",rd,rs1,rs2);
                fprintf(out,"xor \tx%d, x%d, x%d\n",rd,rs1,rs2);
                X[rd] = X[rs1] ^ X[rs2];
                break;
            case 0x5: //SRA and SRL
                switch(funct7){
                    case 0: //SRL
                        printf("srl x%d, x%d, x%d\n",rd,rs1,rs2);
                        fprintf(out,"srl \tx%d, x%d, x%d\n",rd,rs1,rs2);
                        X[rd] = (unsigned int) X[rs1] >> (X[rs2] & 0x1F);
                        break;
                    case 0x20: //SRA
                        printf("sra x%d, x%d, x%d\n",rd,rs1,rs2);
                        fprintf(out,"sra \tx%d, x%d, x%d\n",rd,rs1,rs2);
                        X[rd] = X[rs1] >> (X[rs2] & 0x1F);
                        break;
                }
                break;
            case 0x6: //OR
                printf("or x%d, x%d, x%d\n",rd,rs1,rs2);
                fprintf(out,"or  \tx%d, x%d, x%d\n",rd,rs1,rs2);
                X[rd] = X[rs1] | X[rs2];
                break;
            case 0x7: //AND
                printf("and x%d, x%d, x%d\n",rd,rs1,rs2);
                fprintf(out,"and  \tx%d, x%d, x%d\n",rd,rs1,rs2);
                X[rd] = X[rs1] & X[rs2];
                break;
            }
            pc += 4;
            break;

        case 0x13://I-type opcode=b0010011
            int shamt = imm & 0x1F; //For shifting only uses 5-LSB of IMM value to shift
            switch(funct3){
            case 0x0: //addi
                printf("addi x%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                fprintf(out,"addi \tx%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                X[rd] = X[rs1] + sign_Extend(imm,11);
                break;
            case 0x1: //slli
                printf("slli x%d, x%d, %d\n",rd,rs1,shamt);
                fprintf(out,"slli \tx%d, x%d, %d\n",rd,rs1,shamt);
                X[rd] = X[rs1] << shamt;
                break;
            case 0x2://slti
                printf("slti x%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                fprintf(out,"slti \tx%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                X[rd] = (X[rs1] < sign_Extend(imm,11)) ? 1 : 0; 
                break;
            case 0x3://sltiu - LOOK IF IT NEEDS TO BE CHANGED FOR UNSIGN
                printf("sltiu x%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                fprintf(out,"sltiu \tx%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                X[rd] = (X[rs1] < sign_Extend(imm,11)) ? 1 : 0; 
                break;
            case 0x4://xori
                printf("xori x%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                fprintf(out,"xori \tx%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                X[rd] = X[rs1] ^ sign_Extend(imm,11);
                break;
            case 0x5://srli / srai
                switch(funct7){
                    case 0x0://srli
                        printf("srli x%d, x%d, %d\n",rd,rs1,shamt);
                        fprintf(out,"srli \tx%d, x%d, %d\n",rd,rs1,shamt);
                        X[rd] = X[rs1] >> shamt;
                        break;
                    case 0x20://srai
                        printf("srai x%d, x%d, %d\n",rd,rs1,shamt);
                        fprintf(out,"srai \tx%d, x%d, %d\n",rd,rs1,shamt);
                        X[rd] = X[rs1] >> shamt;
                        break;
                }
                break;
            case 0x6://ori
                printf("ori x%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                fprintf(out,"ori \tx%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                X[rd] = X[rs1] | sign_Extend(imm,11);
                break;
            case 0x7://andi
                printf("andi x%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                fprintf(out,"andi \tx%d, x%d, %d\n",rd,rs1,sign_Extend(imm,11));
                X[rd] = X[rs1] & sign_Extend(imm,11);
                break;
            }
            pc += 4;
            break;
        
        case 0x3://I-type opcode=b0000011
            switch(funct3){
            case 0x0: //lb
                //Sign extend only first byte of data from memory,
                printf("lb x%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                fprintf(out,"lb  \tx%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                X[rd] = sign_Extend((mem[addr]) & 0xFF,7);
                break;
            case 0x1://lh
                //Sign extend second byte of data from memory,
                printf("lh x%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                fprintf(out,"lh  \tx%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                X[rd] = sign_Extend(mem[addr] | (mem[addr + 1] << 8),15);
                break;
            case 0x2://lw
                printf("lw x%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                fprintf(out,"lw  \tx%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                X[rd] = mem[addr] | (mem[addr+1]<<8) | (mem[addr+2]<<16) | (mem[addr+3]<<24);
                break;
            case 0x4://lbu
                //Extract 1 byte
                printf("lbu x%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                fprintf(out,"lbu  \tx%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                X[rd] = mem[addr] & 0xFF;
                break;
            case 0x5://lhu
                //Extract 2 bytes
                printf("lhu x%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                fprintf(out,"lhu \tx%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                X[rd] = (mem[addr] | (mem[addr + 1] << 8)) & 0xFFFF;
                break;

            }
            pc += 4;
            break;
        
        case 0x73: //I-type opcode=b1110011 - ecall, ebreak
            printf("ecall / ebreak\n");
            fprintf(out,"ecall / ebreak\n");
            pc += 4;
            running = 0;
            break;
        
        case 0x67://I-type opcode=b1100111 -jalr
            printf("jalr x%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
            fprintf(out,"jalr \tx%d, %d(x%d)\n",rd,sign_Extend(imm,11),rs1);
            X[rd] = pc + 4;
            pc = (X[rs1] + sign_Extend(imm,11)) & ~0x1;
            break;
        
        case 0x6F://UJ-type opcode=b1101111 - jal
            
            X[rd] = pc + 4;
            imm = (((instr >> 31) & 0x1) << 20) | (((instr >> 12) & 0xFF) << 12) | (((instr >> 20) & 0x1) << 11) | (((instr >> 21) & 0x3FF) << 1);
            pc = pc + (sign_Extend(imm,20));
            printf("jal x%d, %d\n", rd, sign_Extend(imm,20));
            fprintf(out,"jal \tx%d, %d\n", rd, sign_Extend(imm,20));
            break;
        
        case 0x23://S-type opcode=b0100011
            imm = (funct7 << 5) | rd; //12bit imm for type-S/B instructions
            switch(funct3){
            case 0x0: //sb
                printf("sb x%d, %d(x%d)\n",rs2,sign_Extend(imm,11),rs1);
                fprintf(out,"sb  \tx%d, %d(x%d)\n",rs2,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                mem[addr] = X[rs2] & 0xFF;
                break;
            case 0x1: //sh 
                printf("sh x%d, %d(x%d)\n",rs2,sign_Extend(imm,11),rs1);
                fprintf(out,"sh  \tx%d, %d(x%d)\n",rs2,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                mem[addr] = X[rs2] & 0xFF;
                mem[addr+1] = (X[rs2] >> 8) & 0xFF;
                break;
            case 0x2: //sw
                printf("sw x%d, %d(x%d)\n",rs2,sign_Extend(imm,11),rs1);
                fprintf(out,"sw  \tx%d, %d(x%d)\n",rs2,sign_Extend(imm,11),rs1);
                addr = X[rs1] + sign_Extend(imm,11);
                mem[addr] = X[rs2] & 0xFF;
                mem[addr+1] = (X[rs2] >> 8) & 0xFF;
                mem[addr+2] = (X[rs2] >> 16) & 0xFF;
                mem[addr+3] = (X[rs2] >> 24) & 0xFF;
                break;
            }
            pc += 4;
            break;
        
        case 0x63://B-type opcode=b1100011
            //Calculate the correct imm for B-type instructions
            imm = (((instr >> 31) & 0x1) << 12) | (((instr >> 7) & 0x1) << 11) | (((instr >> 25) & 0x3F) << 5) | (((instr >> 8) & 0xF) << 1);
            imm = sign_Extend(imm, 12);
            switch(funct3){
            case 0x0: //beq rs1==rs2
                printf("beq x%d, x%d, %d\n",rs1,rs2,imm);
                fprintf(out,"beq \tx%d, x%d, %d\n",rs1,rs2,imm);
                pc = (X[rs1] == X[rs2]) ? pc + imm : pc + 4;
                break;
            case 0x1: //bne rs1!=rs2
                printf("bne x%d, x%d, %d\n",rs1,rs2,imm);
                fprintf(out,"bne \tx%d, x%d, %d\n",rs1,rs2,imm);
                pc = (X[rs1] != X[rs2]) ? pc + imm : pc + 4;
                break;
            case 0x4: //blt rs1 < rs2 signed
                printf("blt x%d, x%d, %d\n",rs1,rs2,imm);
                fprintf(out,"blt \tx%d, x%d, %d\n",rs1,rs2,imm);
                pc = (X[rs1] < X[rs2]) ? pc + imm : pc + 4;
                break;
            case 0x5: //bge rs1 >= rs2 signed
                printf("bge x%d, x%d, %d\n",rs1,rs2,imm);
                fprintf(out,"bge \tx%d, x%d, %d\n",rs1,rs2,imm);
                pc = (X[rs1] >= X[rs2]) ? pc + imm : pc + 4;
                break;
            case 0x6: // bltu rs1 < rs2 unsigned
                printf("bltu x%d, x%d, %d\n",rs1,rs2,imm);
                fprintf(out,"bltu \tx%d, x%d, %d\n",rs1,rs2,imm);
                pc = ((unsigned int)X[rs1] < (unsigned int)X[rs2]) ? pc + imm : pc + 4;
                break;
            case 0x7: //bgeu rs1 >= rs2 unsigned
                printf("bgeu x%d, x%d, %d\n",rs1,rs2,imm);
                fprintf(out,"bgeu \tx%d, x%d, %d\n",rs1,rs2,imm);
                pc = ((unsigned int)X[rs1] >= (unsigned int)X[rs2]) ? pc + imm : pc + 4;
                break;
            default:
                fprintf(out, "Invalid branch funct3: %d\n", funct3);
                pc = pc + 4;
                break;
            }
            
            break;
        
        case 0x37: //U-type opcode=b0110111 - lui
        // lui - load upper immediate rd = imm << 12
            printf("lui x%d, %#07x\n",rd,(instr & 0xFFFFF000) >> 12);
            fprintf(out,"lui \tx%d, %#07x\n",rd,(instr & 0xFFFFF000) >> 12);
            X[rd] = instr & 0xFFFFF000;
            pc += 4;
            break;
        case 0x17: //U-type opcode=b0010111 - auipc
            // auipc - Add upper immediate to pc. rd = pc + (imm << 12)
            printf("auipc x%d, %#07x\n",rd,(instr & 0xFFFFF000) >> 12);
            fprintf(out,"auipc \tx%d, %#07x\n",rd,(instr & 0xFFFFF000) >> 12);
            X[rd] = pc + (instr & 0xFFFFF000);
            pc += 4;
            break;
        

        case 0xF: //I-type opcode=b0001111 - fence - not required
            pc += 4;
            break;
        
        default: //If unrecognized OPCODE, skip instruction.
            fprintf(out,"SKIPPED INSTRUCTION OPCODE: %#05x\n", opcode);
            pc += 4;
            break;
        }
        if (X[0] != 0) X[0] = 0;
        if (pc >= instr_len) break;
    }
    //prints all values of all registers.
    fprintf(out,"\n\n\t\tREGISTERS\n");
    fprintf(out,"\tREG-ID\t\t    VAL\n");
    for (int i = 0; i < sizeof(X)/sizeof(X[0]);i++){
        printf("Reg: x%d stores: %d\n",i,X[i]);
        fprintf(out,"Reg:  x%d\t stores: %d\n",i,X[i]);
    }
    fprintf(out,"\n\n\t\tFirst kb of memory\n");
    fprintf(out,"____________________________________\n");
    for (int i = 0; i < 1024;i++){
        fprintf(out,"Byte:\t%d\tStores:\t%d\n",i,mem[i]);
    }

    printf("done");
    // while(1){ // Loop to keep in terminal - BC i can

    // }
    return 0;
}
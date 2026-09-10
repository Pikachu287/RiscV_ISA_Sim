#ifndef RV32I
#define RV32I


#ifdef _WIN32
#include <stdint.h>
#define UINT32_T uint32_t
#define UINT16_T uint16_t
#define UINT8_T uint8_t

#define SINT32_T int32_t
#define SINT16_T int16_t
#define SINT8_T int8_t
#endif

#ifdef __unix__
#define UINT32_T __uint32_t
#define UINT16_T __uint16_t
#define UINT8_T __uint8_t

#define SINT32_T __int32_t
#define SINT16_T __int16_t
#define SINT8_T __int8_t
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0


typedef struct {
    UINT8_T * data;
    UINT32_T base;
    UINT32_T size;

}Memory;


typedef struct {
    UINT32_T X[32];
    Memory * mem;
    UINT32_T PC;
    UINT8_T running;
}CPU;

Memory *create_memory(UINT32_T size, UINT32_T base_adress);
void destroy_memory(Memory * mem);

CPU *create_CPU(Memory *mem);
void destroy_CPU(CPU *cpu);

void save_byte(Memory * mem, UINT32_T adress, UINT8_T val);
void save_half(Memory * mem, UINT32_T adress, UINT16_T val);
void save_word(Memory * mem, UINT32_T adress, UINT32_T val);
UINT8_T load_byte(Memory * mem, UINT32_T adress, UINT8_T use_extend);
UINT16_T load_half(Memory * mem, UINT32_T adress, UINT8_T use_extend);
UINT32_T load_word(Memory * mem, UINT32_T adress);

//--------------------------------------------------------------------------------------
//Instruction decoding

typedef enum {
    R_Type,
    I_Type,
    L_Type,//Is actually I-type but for readability L-type is used for load type operations
    ECALL,
    JALR,
    JAL,
    S_Type,
    B_Type,
    LUI,
    AUIPC,
    FENCE,
    UNKNOWN
}Inst_type;
#define OPC_R 0x33
#define OPC_I 0x13
#define OPC_L 0x3
#define OPC_ECALL 0x73
#define OPC_JALR 0x67
#define OPC_JAL 0x6F
#define OPC_S 0x23
#define OPC_B 0x63
#define OPC_LUI 0x37
#define OPC_AUIPC 0x17
#define OPC_FENCE 0xFF


typedef struct {
    UINT32_T raw;
    UINT8_T opcode;
    UINT8_T funct3;
    UINT8_T funct7;
    UINT8_T rd;
    UINT8_T rs1;
    UINT8_T rs2;
    SINT32_T imm;
    Inst_type type;

}Instruction;

SINT32_T sign_Extend(SINT32_T n, SINT32_T extend_bit);
Inst_type decode_Opcode(UINT8_T opcode);
Instruction decode_Instruction(UINT32_T raw_instruction);
void print_type(Inst_type type);

//---------------------------------------------------------------------------------------------------------
//Program execution
void execute_R(CPU * cpu, Instruction * inst);
void execute_I(CPU * cpu, Instruction * inst);
void execute_L(CPU * cpu, Instruction * inst);
void execute_ECALL(CPU * cpu, Instruction * inst);
void execute_JALR(CPU * cpu, Instruction * inst);
void execute_JAL(CPU * cpu, Instruction * inst);
void execute_S(CPU * cpu, Instruction * inst);
void execute_B(CPU * cpu, Instruction * inst);
void execute_LUI(CPU * cpu, Instruction * inst);
void execute_AUIPC(CPU * cpu, Instruction * inst);
void execute_FENCE(CPU * cpu, Instruction * inst);



void execute(CPU * cpu, Instruction * inst);






#endif

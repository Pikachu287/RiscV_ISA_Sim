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

//--------------------------------------------------------------------------------------
//Instruction decoding

typedef enum {
    R_Type,
    I_Type,
    L_Type,
    ECALL,
    JALR,
    JAL,
    S_Type,
    B_Type,
    U_Type,
    FENCE
}inst_type;

typedef struct {
    UINT32_T raw;
    UINT8_T opcode;
    UINT8_T funct3;
    UINT8_T funct7;
    UINT8_T rd;
    UINT8_T rs1;
    UINT8_T rs2;
    UINT32_T imm;
    inst_type type;

}Instruction;

Instruction decode_Instruction(UINT32_T raw_instruction);


void execute(CPU * cpu, Instruction * inst);






#endif

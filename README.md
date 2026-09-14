# RiscV_ISA_Sim
Decoding all RV32I instructions and simulating them

TODO:
- Create CPU typedef 
    - With integrated memory calls.
    - Trap causes??? (DO IT LAST)
    - Running state
    - PC counter
- CPU functions - read/write to register
- Create memory typedef
    - *Data (pointer because of array of values)
    - baseaddress (I guess)
    - Size (1MB)
- Mem read/write functions for 8/16/32 bit sizes
- CPU initialized with memory adress saved in x2(Stack pointer)
- Instruction Typedef - Not neccesary but kinda nice
    - raw data (32bit raw instruction)
    - format (R/S/I/...) (Maybe needed for differentiating function calls)
    - opcode
    - rd, rs1, rs2
    - funct3, funct7
    - imm
- Instruction Decode function instead of large switch in main.




The 32 general-purpose registers
#	    ABI Name    Description	                            Saver
x0	    zero	    Hardwired to 0 (writes ignored)	        —
x1	    ra	        Return address	                        Caller
x2	    sp      	Stack pointer	                        Callee
x3	    gp      	Global pointer	                        —
x4	    tp      	Thread pointer	                        —
x5	    t0      	Temporary register 0	                Caller
x6	    t1      	Temporary register 1	                Caller
x7	    t2      	Temporary register 2	                Caller
x8	    s0/fp   	Saved register 0 / frame pointer	    Callee
x9	    s1      	Saved register 1	                    Callee
x10	    a0      	Function argument 0 / return value 0	Caller
x11	    a1      	Function argument 1 / return value 1	Caller
x12	    a2      	Function argument 2	                    Caller
x13	    a3      	Function argument 3	                    Caller
x14	    a4      	Function argument 4	                    Caller
x15	    a5      	Function argument 5	                    Caller
x16	    a6      	Function argument 6	                    Caller
x17	    a7      	Function argument 7 / syscall number	Caller
x18	    s2      	Saved register 2	                    Callee
x19	    s3      	Saved register 3	                    Callee
x20	    s4      	Saved register 4	                    Callee
x21	    s5      	Saved register 5	                    Callee
x22	    s6      	Saved register 6	                    Callee
x23	    s7      	Saved register 7	                    Callee
x24	    s8      	Saved register 8	                    Callee
x25	    s9      	Saved register 9	                    Callee
x26	    s10     	Saved register 10	                    Callee
x27	    s11     	Saved register 11	                    Callee
x28	    t3      	Temporary register 3	                Caller
x29	    t4      	Temporary register 4	                Caller
x30	    t5      	Temporary register 5	                Caller
x31	    t6      	Temporary register 6	                Caller


ECALLS:
1: print_int
4: print_string
9: sbrk
10: exit
11: print_character
17: exit2 



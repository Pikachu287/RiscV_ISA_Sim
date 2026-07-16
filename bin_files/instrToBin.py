import struct
# List of instructions
# instructions = [
#     0x00200093, 0x002081b3, 0x00300113, 0x00811213,
#     0x00125213, 0x00202023, 0x00002503, 0x004000a3,
#     0x00102583,
# ]
instructions = [
    0x003100B3,#R-type
    0x403100B3,
    0x003110B3,
    0x003120B3,
    0x003130B3,
    0x003140B3,
    0x003150B3,
    0x403150B3,
    0x003160B3,
    0x003170B3,#Last R-type
    0x00A10093,#I-type
    0xFFF00293, #Extra addi for neg value
    0x00A12093,
    0x00A13093,
    0x00A14093,
    0x00A16093,
    0x00A17093,#Last I arith
    0x00311093, #I-type shift
    0x00315093,
    0x40315093,#Last I shift
    0x00410083, #Loads
    0x00411083,
    0x00412083,
    0x00414083,
    0x00415083,#Last load
    # 0x008100E7, #jalr
    0x00310223, #S-type
    0x00311223,
    0x00312223, #Last S-type
    # 0x00208663, #B-type branching
    # 0x00209663,
    # 0x0020C663,
    # 0x0020D663,
    # 0x0020E663,
    # 0x0020F663, #Last B-type
    0x123450B7, #U-type - lui
    0x12345097 #auipc
    # 0x010000EF, #J-type jal    
]
# Convert all instructions to instructions.bin file
with open("instructions.bin", "wb") as f:
    for instr in instructions:
        f.write(struct.pack("<I", instr))
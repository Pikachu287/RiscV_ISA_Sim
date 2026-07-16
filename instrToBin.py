import struct
# List of instructions
instructions = [
    0x00200093, 0x002081b3, 0x00300113, 0x00811213,
    0x00125213, 0x00202023, 0x00002503, 0x004000a3,
    0x00102583,
]
# Convert all instructions to instructions.bin file
with open("instructions.bin", "wb") as f:
    for instr in instructions:
        f.write(struct.pack("<I", instr))
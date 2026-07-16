#!/usr/bin/env python3
"""
Decode a raw .res register-dump file into human-readable text.

Assumes the file is a raw binary dump of N 4-byte little-endian signed
integers, written directly from something like:

    fwrite(X, sizeof(int), 32, fptr);

For a 32-register RISC-V simulator (int X[32]), this is exactly
32 * 4 = 128 bytes -- which matches the size of the sample .res file.

Usage:
    python3 res_to_readable.py
    (then type the filename when prompted, e.g. loop.res)

    Optional flags still work if you want to skip the prompt:
    python3 res_to_readable.py -f input.res
    python3 res_to_readable.py -f input.res -o output.txt
    python3 res_to_readable.py -f input.res --count 32   # override register count
"""

import argparse
import os
import struct
import sys


def decode_res_file(path, count=None, signed=True, little_endian=True):
    with open(path, "rb") as f:
        data = f.read()

    if len(data) % 4 != 0:
        print(f"Warning: file size ({len(data)} bytes) is not a multiple of 4; "
              f"trailing {len(data) % 4} byte(s) will be ignored.", file=sys.stderr)

    n_values = len(data) // 4

    if count is not None and count != n_values:
        print(f"Warning: expected {count} values based on --count, "
              f"but file contains {n_values} 4-byte values. Using {n_values}.",
              file=sys.stderr)

    endian_char = "<" if little_endian else ">"
    type_char = "i" if signed else "I"
    fmt = f"{endian_char}{n_values}{type_char}"

    values = struct.unpack(fmt, data[: n_values * 4])
    return values


def format_output(values):
    lines = []
    lines.append("Register dump")
    lines.append("=" * 40)
    for i, v in enumerate(values):
        # Show as unsigned 32-bit hex too, since register contents are
        # often easier to read in hex (addresses, bit patterns, etc.)
        hex_val = v & 0xFFFFFFFF
        if chr(v) == '\n':
            lines.append(f"x{i:<2}  =  {v:>12}   (0x{hex_val:08x})  ASCII = \\n")
        else:
            lines.append(f"x{i:<2}  =  {v:>12}   (0x{hex_val:08x})  ASCII = {chr(v)}")
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                      formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("-f", "--file", dest="input", default=None,
                         help="Path to the .res file. If omitted, you'll be prompted for it.")
    parser.add_argument("-o", "--output", help="Write output to this file instead of stdout")
    parser.add_argument("--count", type=int, default=None,
                         help="Expected number of values (e.g. 32 for a register file). "
                              "Only used to sanity-check the file size; does not truncate.")
    parser.add_argument("--unsigned", action="store_true",
                         help="Interpret values as unsigned instead of signed")
    parser.add_argument("--big-endian", action="store_true",
                         help="Interpret values as big-endian instead of little-endian")
    args = parser.parse_args()

    # If no filename was given on the command line, ask for it interactively.
    # Keep asking until a file is found, instead of crashing on a bad path.
    while True:
        if args.input is None:
            args.input = input("Enter the .res filename (e.g. loop.res): ").strip().strip('"')

        try:
            values = decode_res_file(
                args.input,
                count=args.count,
                signed=not args.unsigned,
                little_endian=not args.big_endian,
            )
            break
        except FileNotFoundError:
            print(f"Could not find '{args.input}' in the current folder "
                  f"({os.getcwd()}).")
            print("Tip: either type the full path (e.g. C:\\path\\to\\loop.res "
                  "or bin_files/loop.res), or cd into the folder that contains it first.")
            args.input = None  # clear so we prompt again

    output = format_output(values)

    if args.output:
        with open(args.output, "w") as f:
            f.write(output + "\n")
        print(f"Wrote {len(values)} values to {args.output}")
    else:
        print(output)


if __name__ == "__main__":
    main()
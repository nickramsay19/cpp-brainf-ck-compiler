#!/bin/bash

INPUT="$1"
OUTPUT_BC='output.bc'
OUTPUT_OBJ='output.o'
OUTPUT='output'

llvm-as -o "$OUTPUT_BC" "$INPUT"

llc -filetype='obj' -o "$OUTPUT_OBJ" "$OUTPUT_BC"

clang -o "$OUTPUT" "$OUTPUT_OBJ"


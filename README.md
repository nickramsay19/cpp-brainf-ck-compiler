# Brainfuck LLVM Compiler
> Nicholas Ramsay

Transpiles [Brainfuck](https://esolangs.org/wiki/Brainfuck) code into LLVM IR, and subsequently, a compiled binary executable. LLVM IR is a standardised representation of code whereby a slew of existing back-end compiler optimizations are already avalaible.

Since Brainfuck syntax comprises exclusively single character tokens, no *scanner*/*tokenizer* is needed. Instead the *parser* **LL(1)** parses directly over a (subclass of) `std::ostream`.

The CLI is designed to be as identical to `clang` as possible. So, if you are familiar with `clang`'s `-emit-llvm` and the `-S`/`-c` options, there is nothing new to learn, the output types are the same.

Additionally, for both input and output file arguments, a "-" denotes `stdin` and `stdout` respectively.

## Usage
```
Usage: bfc [--help] [--output VAR] [[--asm]|[--compile]|[--exe]] [--emit-llvm] input

Positional arguments:
  input                       Input file name [default: "-"]

Optional arguments:
  -h, --help                  shows help message and exits 
  -v, --version               prints version information and exits 
  -o, --output                Output executable file name [default: "a.out"]

Stage Selection Options:
  -S, --asm                   LLVM generation and optimization stages and target-specific code generation, producing an assembly file 
  -c, --compile               The above, plus the assembler, generating a target ".o" object file. 
  -e, --exe                   The above, plus linking to produce an executable [default]

Code Generation Options:
  -l, --llvm-ir, --emit-llvm  Emit LLVM IR 
```

### Simple usage
```
bin/bfc -lS -o output.ll input.bf
sh scripts/make-exe.sh output.ll

chmod +x output
./output
```

## Compiling
1. Ensure `llvm` headers are installed
2. Ensure `llvm-config` is installed (it should be if you have `llvm`)
3. Simply run `make`, compiled project is in `bin/bfc`

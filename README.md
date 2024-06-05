# Brainfuck Compiler
> Nicholas Ramsay

Transpiles [Brainfuck](https://esolangs.org/wiki/Brainfuck) code into LLVM IR, and subsequently, a compiled binary executable.

Since Brainfuck syntax comprises exclusively single character tokens, no *scanner* or *tokenizer* is needed. Instead the *parser* **LL(1)** parses directly over a `std::fstream` object.

## Usage
```
Usage: bfc [--help] [--version] [--output VAR] input

Positional arguments:
  input          input brainfuck file name [nargs=0..1] [default: "-"]

Optional arguments:
  -h, --help     shows help message and exits 
  -v, --version  prints version information and exits 
  -o, --output   output executable file name [nargs=0..1] [default: "a.out"]
```

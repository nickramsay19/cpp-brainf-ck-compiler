# Brainfuck Compiler
> Nicholas Ramsay

Transpiles [Brainfuck](https://esolangs.org/wiki/Brainfuck) code into LLVM IR, and subsequently, a compiled binary executable.

Since Brainfuck syntax comprises exclusively single character tokens, no *scanner* or *tokenizer* is needed. Instead the *parser* **LL(1)** parses directly over a `std::fstream` object.

## Usage
```
bfc -o main input.bf

# will default to 'a.out' if -o not given
bfc input.bf

# write output to stdout
bfc -o - input.bf
```

### Input from `stdin`
```
cat input.bf | bfc
```

### Output formats
#### Emit LLVM IR
```
bfc -l input.bf
```

#### Emit LLVM bitcode
```
bfc -b input.bf

# can do both 
bf -lb input.bf
```


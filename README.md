# NanoG Compiler (Educational)

 - NanoG Compiler is a Nano sized compiler written C Language
 - This compiler can translate NanoG source into a pure-readable NASM 64bit Linux assembly
 - This is not a fully completed language
 - And no Documents

```ng
fn program() -> int {
  int a = 10;
  int b = 20;

  return a + b;
}
```

 - Main function of the language is `program`, not `main` or `_start`

## Required Platform/Tool(s)

 - Linux OS (like Ubuntu)
 - x86_64 Processor Architecture
 - GCC (C Compiler): `sudo spt-get install gcc`
 - LD (Linker): `sudo apt-get install ld`
 - NASM (Assembler): `sudo apt-get install nasm`

**More Info**
 - [NASM Assembly Note - https://github.com/las-nish/NASM-Assembly-Collection](https://github.com/las-nish/NASM-Assembly-Collection)

## Compile Project

Location: `/project/`

```bash
make BUILD
```

## Usage

Location: `Project/Export/nano_g`

```bash
./nano_g [command] [file]
```

**[command]**
 - `-r`: Generate Assembly File
 - `-r-o`: Generate Object File
 - `-t`: Tokenize File

**[file]**
 - Name of the Input file

## License

```
Copyright (c) 2020-2021 Lasan Nishshanka (@las-nish)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

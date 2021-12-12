[![Main](https://github.com/liubai01/Lua-Lab1-luacFormatter/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/liubai01/Lua-Lab1-luacFormatter/actions/workflows/c-cpp.yml) ![](https://img.shields.io/github/v/release/liubai01/Lua-Lab1-luacFormatter)![](https://img.shields.io/github/last-commit/liubai01/Lua-Lab1-luacFormatter)

# Lua Lab1 LuacFormatter

**Author**: `Yintao, Xu`

**Email**: `yintao.xu@nyu.edu`

In this lab, we aim at parsing lua5.3's dumped file(a.k.a lua byte code). Our goal is to manipulate lua's assembly code and get familiar with the source code. [Chunkspy.lua](https://github.com/efrederickson/LuaAssemblyTools) as reference provides similar functionality.

## Usage

`./luacformatter <file> <options>`

- **-h**: Print formatted header block.
- **-f**: Print function block

## Example

Example of a `do end` program into dumped file. Our goal is to understand and decode this binary file.

```bash
xxd minimal.luac
00000000: 1b4c 7561 5300 1993 0d0a 1a0a 0408 0408  .LuaS...........
00000010: 0878 5600 0000 0000 0000 0000 0000 2877  .xV...........(w
00000020: 4001 0d40 6d69 6e69 6d61 6c2e 6c75 6100  @..@minimal.lua.
00000030: 0000 0000 0000 0000 0202 0100 0000 2600  ..............&.
00000040: 8000 0000 0000 0100 0000 0100 0000 0000  ................
00000050: 0100 0000 0100 0000 0000 0000 0100 0000  ................
00000060: 055f 454e 56                             ._ENV
```

Parsed by `luacformatter`:

```bash
$ ./luacformatter luas/minimal.luac -hf
| Offset     | Content      | Desciption                           |
| ---------- | ------------ | ------------------------------------ |
|            |              |                                      | 
|            |              | **Lua Metadata**                     | 
|            |              |                                      | 
| 0x00000000 | 0x1b4c7561   | Lua signature                        | 
| 0x00000004 | 0x53         | Lua version                          | 
| 0x00000005 | 0x00         | Lua Format Version                   | 
| 0x00000006 | 0x19930d0a1a | Lua Data                             | 
|            | 0a           |                                      | 
|            |              |                                      | 
|            |              | **Lua Metadata(Sizes)**              | 
|            |              |                                      | 
| 0x0000000c | 0x04         | Size of Integer                      | 
| 0x0000000d | 0x08         | Size of size_t                       | 
| 0x0000000e | 0x04         | Size of Instruction                  | 
| 0x0000000f | 0x08         | Size of Lua Integer                  | 
| 0x00000010 | 0x08         | Size of Lua Number                   | 
|            |              |                                      | 
|            |              | **Lua Examples**                     | 
|            |              |                                      | 
| 0x00000011 | 22136        | Example Lua Integer (0x5678=22146)   | 
| 0x00000019 | 370.500000   | Example Lua Number (370.5)           | 
|            |              |                                      | 
| 0x00000021 | 0x01         | Size of Up Values                    | 
|            |              |                                      | 
| 0x00000022 |              | End of table                         | 

Table Size: 34

Function[0]: Level 0
| Offset     | Content      | Desciption                           |
| ---------- | ------------ | ------------------------------------ |
|            |              |                                      | 
|            |              | **Lua Function Metadata**            | 
|            |              |                                      | 
| 0x00000022 | @luas/minima | Source name                          | 
|            | l.lua        |                                      | 
| 0x00000034 | 0x00000000   | First line defined                   | 
| 0x00000038 | 0x00000000   | Last lind defined                    | 
| 0x0000003c | 0            | Number of params                     | 
| 0x0000003d | 0x02         | `is_vararg` flag                     | 
| 0x0000003e | 2            | Maximum stack size                   | 
|            |              |                                      | 
|            |              | **Function Instructions**            | 
|            |              |                                      | 
| 0x0000003f | 1            | Number of instructions               | 
| 0x00000047 | 0x26008000   | instr 0, optcode 38                  | 
|            |              |                                      | 
|            |              | **Constants**                        | 
|            |              |                                      | 
| 0x00000047 | 0            | Number of constants                  | 
|            |              |                                      | 
|            |              | **UpValues**                         | 
|            |              |                                      | 
| 0x0000004b | 1            | Number of upvalues                   | 
|            |              | Upvalue [0]                          | 
| 0x0000004f | 1            | is instack                           | 
| 0x00000050 | 0            | upval index                          | 
|            |              |                                      | 
|            |              | **Nested Functions**                 | 
|            |              |                                      | 
| 0x00000051 | 0            | Nested Function Nums                 | 
|            |              |                                      | 
|            |              | **Debug Infos - Line Info(opcode ->  | 
|            |              | src. line)**                         | 
|            |              |                                      | 
| 0x00000059 | op code [0]  | source code line [1]                 | 
|            |              |                                      | 
|            |              | **Debug Infos - Local Variables**    | 
|            |              |                                      | 
| 0x0000005d | 0            | Number of local variables            | 
|            |              |                                      | 
|            |              | **Debug Infos - Up Values Names**    | 
|            |              |                                      | 
| 0x00000061 | 1            | Number of up values                  | 
| 0x00000065 | _ENV         | [0] Name                             | 
|            |              |                                      | 
| 0x00000065 |              | End of table                         | 

Table Size: 72
```

## Road map

1. [What is lua dumped file? How to get it from source code?](docs/luadump.md)
2. [Lua Dumped Files Layout: header block and top-level function](docs/lualayout.md)

## Reference
1. A No-Frills Introduction to Lua 5.1 VM instructions by Kein-Hong Man, esq.
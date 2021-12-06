# LuaLab1-Parser
**Author**: `Yintao, Xu`

**Email**: `yintao.xu@nyu.edu`

In this lab, we aim at parsing lua5.3's dumped file(a.k.a lua byte code). Our goal is to manipulate lua's assembly code and get familiar with the source code. [Chunkspy.lua](https://github.com/efrederickson/LuaAssemblyTools) as reference provides similar functionality.

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

## Roadmap

1. [What is lua dumped file? How to get it from source code?](docs/luadump.md)
2. [Lua Dumped Files Layout: header block and top-level function](docs/lualayout.md)

## Reference
1. A No-Frills Introduction to Lua 5.1 VM instructions by Kein-Hong Man, esq.
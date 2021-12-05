# LuaLab1-Parser
**Author**: `Yintao, Xu`

**Email**: `yintao.xu@nyu.edu`

One milestone to write interpreter of Lua5.3

## Lua Dumped File
**Motivations**: Lua interpreter translates `.lua` code into lua instructions, than execuated on Lua virtual machine. We aim aim at constructing a VM ourselves. First, it is important to get assembly code in lua instructions.

**Luac**: Lua can dump functions as binary chunks. The dumped file could be got by two ways:

1. **`luac` command**: `luac -o <output_filename> <luasource_filename>`
2. **Invoke CAPI at `lapi.h`**: `LUA_API int lua_dump (...) `

**Remark**: The first method is easier, but `luac` is excluded from lua's official [github repository](github.com/lua/lua). If you compiled from source, luac would be missing. `Apt-get` verision in ubuntu would bring you with luac.

**Example** : dump lua script by luac:

```bash
liubai01@liubai01-VirtualBox:~/Desktop/luactest$ echo "do end" > minimal.lua
liubai01@liubai01-VirtualBox:~/Desktop/luactest$ luac5.3 -o minimal.luac minimal.lua
liubai01@liubai01-VirtualBox:~/Desktop/luactest$ cat minimal.lua
do end
liubai01@liubai01-VirtualBox:~/Desktop/luactest$ xxd minimal.luac
00000000: 1b4c 7561 5300 1993 0d0a 1a0a 0408 0408  .LuaS...........
00000010: 0878 5600 0000 0000 0000 0000 0000 2877  .xV...........(w
00000020: 4001 0d40 6d69 6e69 6d61 6c2e 6c75 6100  @..@minimal.lua.
00000030: 0000 0000 0000 0000 0202 0100 0000 2600  ..............&.
00000040: 8000 0000 0000 0100 0000 0100 0000 0000  ................
00000050: 0100 0000 0100 0000 0000 0000 0100 0000  ................
00000060: 055f 454e 56                             ._ENV
```

## Lua Binary Chunks
Refer to *A No-Frills Introduction to Lua 5.1 VM instructions by Kein-Hong Man, esq*[1], a binary consists of two parts: a header block and a top-level function. 

**Remark**: There is no official docs to introduce the dumped lua. The dumped logic is at `ldump.c`. You could refer to `luaU_dump` function as an entry.

**Remark**: lua 5.3's binary dumped file is differently from 5.1 and other version. Therefore, the tutorial here is inconsistent with Kein-Hong Man's manual[1].

**Header block layout**:

| Size    | Description                                                  |
| ------- | ------------------------------------------------------------ |
| 4 bytes | Header signature: ESC, “Lua” or 0x1B4C7561                   |
| 1 byte  | Version number, 0x53 (81 decimal) for Lua 5.3                |
| 1 byte  | Format version, 0=official version                           |
| 6 bytes | Data To Catch Conversion Errors. (by default: `0x19930d0a1a0a`) |
|         |                                                              |
| 1 byte  | Size of integer (in bytes) (default 4)                       |
| 1 byte  | Size of size_t (in bytes) (default 8 on 64-bits OS)          |
| 1 byte  | Size of Instruction (in bytes) (default 4)                   |
| 1 byte  | Size of lua_Integer (in bytes) (default 8)                   |
| 1 byte  | Size of lua_Number (in bytes) (default 8)                    |
|         |                                                              |
| 8 bytes | Lua Example Integer. (0x5678)                                |
| 8 bytes | Lua Example Number(a double). (370.5)                        |
|         |                                                              |
| 1 byte  | Size of Up values. (default 1)                               |


## Reference
1. A No-Frills Introduction to Lua 5.1 VM instructions by Kein-Hong Man, esq.
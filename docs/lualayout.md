## Lua Binary Chunks
Refer to *A No-Frills Introduction to Lua 5.1 VM instructions by Kein-Hong Man, esq*[1], a binary consists of two parts: a **header block** and a **top-level function**. 

**Remark**: There is no official docs to introduce the dumped lua. The dumped logic is at `ldump.c`. You could refer to `luaU_dump` function as an entry.

**Remark**: lua 5.3's binary dumped file is differently from 5.1 and other versions. Therefore, the tutorial here is inconsistent with Kein-Hong Man's manual based on 5.1.

### Header block layout

The dumped lua binary files start with header block.

| Size        | Description                                                  |
| ----------- | ------------------------------------------------------------ |
| 4 bytes     | Header signature: ESC, “Lua” or 0x1B4C7561                   |
| 1 byte      | Version number, 0x53 (81 decimal) for Lua 5.3                |
| 1 byte      | Format version, 0=official version                           |
| 6 bytes     | Data To Catch Conversion Errors. (by default: `0x19930d0a1a0a`) |
|             |                                                              |
| 1 byte      | Size of integer (in bytes) (default 4)                       |
| 1 byte      | Size of size_t (in bytes) (default 8 on 64-bits OS)          |
| 1 byte      | Size of Instruction (in bytes) (default 4)                   |
| 1 byte      | Size of lua_Integer (in bytes) (default 8)                   |
| 1 byte      | Size of lua_Number (in bytes) (default 8)                    |
|             |                                                              |
| Lua_Integer | Lua Example Integer. (0x5678)                                |
| Lua_Number  | Lua Example Number(a double). (370.5)                        |
|             |                                                              |
| 1 byte      | Size of Up values. (default 1)                               |

By default on 64-bit machines, `Lua_Integer` is 8 bytes(long) and `Lua_Number` is also 8 bytes(double).

**Example:**

```bash
    Offset               Content            Desciption
======================================================
                                                      
                                  Lua Metadata        
                                                      
0x00000000  0x1b4c7561            Lua signature       
0x00000004  0x53                  Lua version         
0x00000005  0x00                  Lua Format Version  
0x00000006  0x19930d0a1a0a        Lua Data            
                                                      
                                  Lua Metadata(Sizes) 
                                                      
0x00000007  0x04                  Size of Integer     
0x00000008  0x08                  Size of size_t      
0x00000009  0x04                  Size of Instruction 
0x0000000a  0x08                  Size of Lua Integer 
0x0000000b  0x08                  Size of Lua Number  
0x0000000c  0x01                  Size of Up Values   
0x0000000d  22136                 Size of Example Lua 
                                  Integer (0x5678=2214
                                  6)                  
0x00000015  370.500000            Size of Example Lua 
                                  Number (370.5)      
                                                      
0x0000001d                        End of table  
```

### Top-level function block

After head block, here comes the top-level function block. 

Top-level function block's layout is as follows(so does normal function blocks):

| Size               | Description                                       |
| ------------------ | ------------------------------------------------- |
| String             | Source name                                       |
| 4 bytes            | Line first defined in source file (Int)           |
| 4 bytes            | Line Last defined in source file (Int)            |
|                    | Parameter List                                    |
| 1 byte             | Number of parameters (Int)                        |
| 1 byte             | `is_vararg` flag (`=1` if has variable arguments) |
| 1 byte             | Max Stack Size                                    |
|                    | Assembly Instructions List (opcodes)              |
| 4 bytes            | Number of Instructions (Int)                      |
| [4 bytes]*         | Assembly Instructions                             |
|                    | Constant List                                     |
| 4 bytes            | Number of Constants (Int)                         |
| [TValue]*          | Constant supports multiple types(Details below)   |
|                    | Up value List                                     |
| 4 bytes            | Number of Up values (Int)                         |
| [2 bytes]*         | Up values Info(in-stack flag and global index)    |
|                    | Nested Function List                              |
| 4 bytes            | Number of Nested Functions (Int)                  |
| [function block]*  | Same layout as top-level function block           |
|                    | (Debug) Map from opcodes to source lines          |
| 4 bytes            | Number of line mapping (Int)                      |
| [4 bytes]*         | An integer, map from opcodes to source lines      |
|                    | (Debug) Local Variables Name List                 |
| 4 bytes            | Number of local variables (Int)                   |
| [String + 8bytes]* | Local Variable Names + `startpc` + `endpc`        |
|                    | (Debug) Up Values Name List                       |
| 4 bytes            | Number of Up values (Int)                         |
| [String]*          | Up value Names                                    |

**Remark**: `[...]*` means that it could be multiple values here in the list.

#### String Layout

String' s layout depends on its size. 

1. **Short String**: Size of string(include last `\0` end sign) is < 0xFF (size could be saved in a byte).
2. **Long String**: Size of string(include last `\0` end sign) is >= 0xFF (size could not be saved in a byte).

Short String Layout(denoted by LUA_TSHRSTR in lua):

| Size             | Description                         |
| ---------------- | ----------------------------------- |
| 1 byte           | String Size (includes tailing `\0`) |
| `size - 1` bytes | String without tailing `\0`         |

Long String Layout(denoted by LUA_TLNGSTR in lua):

| Size             | Description                                 |
| ---------------- | ------------------------------------------- |
| 1 byte           | 0XFF to indicate size overflows in one byte |
| size_t           | String Size (includes tailing `\0`)         |
| `size - 1` bytes | String without tailing `\0`                 |

**Remark**: `size_t` is 8 bytes in 64 bits machine and 4 bytes in 32 bits machine.

**Example**: a short string layout(in hex) example of `@luas/minimal.lua` (size of 18)

```bash
1240 6c75 6173 2f6d 696e 696d 616c 2e6c  .@luas/minimal.l
7561                                     ua
```

#### TValue Layout

Constant is stored in TValue structure in lua5.3 implementation. It follows

| Size   | Description                          |
| ------ | ------------------------------------ |
| 2 bits | Non variant type tag (details below) |
| 2 bits | Variant type tag                     |
|        | Value content                        |

**Non variant type tag and its size**

- **0**: `Nil` (0 byte)
- **1**: `Bool` (1 byte)
- **3**: `Lua Double` (8 bytes)/`Lua Integer` (8 bytes)
- **4**: `String`(layout same as above)

**Variant type tag**:

- **nonvariant=3(double/int)**: `vartag=0`->`Lua Double`, `vartag=1`->`Lua Integer`
- **nonvariant=4(string)**: `vartag=0`->short string, `vartag=1`->long string (layout same as above)


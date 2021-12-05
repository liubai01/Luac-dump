# LuaLab1-Parser
**Author**: Yintao, Xu
One milestone to write interpreter of Lua5.3

## Lua Dumped File
**Motivations**: Lua interpreter translates `.lua` code into lua instructions, than execuated on Lua virtual machine. We aim aim at constructing a VM ourselves. First, it is important to get assembly code in lua instructions.

**Luac**: Lua can dump functions as binary chunks. The dumped file could be got by two ways:

1. **`luac` command**: `luac <luasource_filename> -o <output_filename>`
2. **Invoke CAPI at `lapi.h`**: `LUA_API int lua_dump (lua_State *L, lua_Writer writer, void *data, int strip) `

**Remark**: The first method is easier, but `luac` is excluded from lua's official [github repository](github.com/lua/lua). If you compiled from source, luac would be missing. `Apt-get` verision in ubuntu would bring you with luac.

## Lua Binary Chunks
Refer to *A No-Frills Introduction to Lua 5.1 VM instructions by Kein-Hong Man, esq*[1], a binary consists of two parts: a header block and a top-level function.

**Remark**: lua 5.3's binary dumped file is differently from 5.1 and other version. Therefore, the tutorial here is inconsistent with Kein-Hong Man's manual[1].


## Reference
1. A No-Frills Introduction to Lua 5.1 VM instructions by Kein-Hong Man, esq.
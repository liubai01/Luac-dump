## Lua Dumped File
**Motivations**: Lua interpreter translates `.lua` code into lua instructions, than execuated on Lua virtual machine. We aim aim at constructing a VM ourselves. First, it is important to get assembly code in lua instructions.

**Luac**: Lua can dump functions as binary chunks. The dumped file could be got by two ways:

1. **`luac` command**: `luac -o <output_filename> <luasource_filename>`
2. **Invoke CAPI at `lapi.h`**: `LUA_API int lua_dump (...) `

**Remark**: The first method is easier, but `luac` is excluded from lua's official [github repository](https://github.com/lua/lua). If you compiled from source, luac would be missing. `Apt-get` verision in ubuntu would bring you with luac.

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
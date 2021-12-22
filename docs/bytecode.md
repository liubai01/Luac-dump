**Lua VM Bytecode Table**(refer to [this manual](https://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html))

|      | Opcode   | Description                                         |
| ---- | -------- | --------------------------------------------------- |
| 0    | MOVE     | Copy a value between registers                      |
| 1    | LOADK    | Load a constant into a register                     |
| 2    | LOADKX   | Load a constant into a register (#const. >  262143) |
| 3    | LOADBOOL | Load a boolean into a register                      |
| 4    | LOADNIL  | Load nil values into a range of registers           |
| 5    | GETUPVAL | Read an upvalue into a register                     |
| 6    | GETTABUP | Read a value from table in up-value into a register |
| 7    | GETTABLE | Read a table element into a register                |
| 8    | SETTABUP | Write a register value into table in up-value       |
| 9    | SETUPVAL | Write a register value into an upvalue              |
| 10   | SETTABLE | Write a register value into a table element         |
| 11   | NEWTABLE | Create a new table                                  |
| 12   | SELF     | Prepare an object method for calling                |
| 13   | ADD      | Addition operator                                   |
| 14   | SUB      | Subtraction operator                                |
| 15   | MUL      | Multiplication operator                             |
| 16   | MOD      | Modulus (remainder) operator                        |
| 17   | POW      | Exponentation operator                              |
| 18   | DIV      | Division operator                                   |
| 19   | IDIV     | Integer division operator                           |
| 20   | BAND     | Bit-wise AND operator                               |
| 21   | BOR      | Bit-wise OR operator                                |
| 22   | BXOR     | Bit-wise Exclusive OR operator                      |
| 23   | SHL      | Shift bits left                                     |
| 24   | SHR      | Shift bits right                                    |
| 25   | UNM      | Unary minus                                         |
| 26   | BNOT     | Bit-wise NOT operator                               |
| 27   | NOT      | Logical NOT operator                                |
| 28   | LEN      | Length operator                                     |
| 29   | CONCAT   | Concatenate a range of registers                    |
| 30   | JMP      | Unconditional jump                                  |
| 31   | EQ       | Equality test, with conditional jump                |
| 32   | LT       | Less than test, with conditional jump               |
| 33   | LE       | Less than or equal to test, with conditional jump   |
| 34   | TEST     | Boolean test, with conditional jump                 |
| 35   | TESTSET  | Boolean test, with conditional jump and assignment  |
| 36   | CALL     | Call a closure                                      |
| 37   | TAILCALL | Perform a tail call                                 |
| 38   | RETURN   | Return from function call                           |
| 39   | FORLOOP  | Iterate a numeric for loop                          |
| 40   | FORPREP  | Initialization for a numeric for loop               |
| 41   | TFORLOOP | Iterate a generic for loop                          |
| 42   | TFORCALL | Initialization for a generic for loop               |
| 43   | SETLIST  | Set a range of array elements for a table           |
| 44   | CLOSURE  | Create a closure of a function prototype            |
| 45   | VARARG   | Assign vararg function arguments to registers       |
| 46   | EXTRAARG | Provide extraarg for loadkx                         |
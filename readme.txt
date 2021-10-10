The code prints certain error messages in certain unexpected
cases.

If the number of arguments is less than two, the error message printed is
"Too less arguments. Give file name as command line argument."

When file could not be opened, the error message is 
"File could not be opened. "

If the opcode field bit is wrong, the error message printed is 
"Binary does not corresponds to any opcode. Check the input file."

If the type two field bit is wrong, the error message printed is 
"Binary does not corresponds to any value/ register/ stack/ pointer. 
Check the input file."

The code prints appropriate error messages when functions like fseek, and ftell fail.

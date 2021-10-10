#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define RAM_SIZE 256
#define RAM_ARRAY_SIZE 254
#define NUMBER_OF_REGISTERS 8
#define ASCII_a 97
#define ASCII_0 48
#define ASCII_A 65
#define SMALL_BUF 16
#define BUF 50
#define NUM_OF_ALPHABETS 26


/*
This function returns the position, where the set of stack symbols 
for the next function call are supposed to be stored. The stack grows
 from the end of the ram/ memory. The free/ empty memory locations of the
  ram are denoted by character 'w'.
 
 parameter: 
 ram -> ram of the architecture. (unsigned char array)
 
 return: 
 the position from where the next set of stack symbols should
 start. (unsigned int)
*/
unsigned int stack_offset (unsigned char* ram) {
	unsigned int i = RAM_ARRAY_SIZE - 1;
	for (i = RAM_ARRAY_SIZE - 1; i >= 0; i--) {
		if (ram [i] == 'w') {
			return i;
		}
	}
}

/*
This function resets/ frees the stack memory locations after return from
 the current function call. 
 
 parameters:
 ram -> ram of the architecture. (unsigned char array)
 offset -> the position from where the stack symbols started growing for
  the current function call. (unsigned int)
 
 return:
 It does not returns anything. It just clears the current stack. It is 
 called when we return from the function call. (void)
*/
void stack_reset (unsigned char* ram, unsigned int offset) {
	unsigned int i = offset;
	for (i = offset; i >= 0; i--) {
		if (ram [i] == 'w') {
			break;
		}
		ram [i] = 'w';
	}
}

/*
This function returns the value associated with the register address,
 stack symbol, pointer value, or value accordingly. For simplicity, our
  architecture stores type of value using different 
 characters. Register adresses are stored as p, value is stored as q,
  stack symbol is stored as r, and pointer value as s. 
  
  parameters:
  i -> it stores the position in memory where the corresponding operation
  to be performed is stored. All the value types, and value are accessed
  based on their position relative to the operation. (int)
  ram -> ram of the architecture. (unsigned char array)
  registers -> the registers in the architecture. (unsigned char array)
  offset -> the position from where the stack symbols started growing for
   the current function call. (unsigned int)
  
  return:
  the value associated with the particular value type. (unsigned int)
*/
unsigned int get_value (int i, unsigned char* ram, unsigned char* registers, unsigned int offset) {
			if (ram [i + 1] == 'p') {
				return (unsigned int) registers [(unsigned int)ram[i+2] - ASCII_0] ;
			} else if (ram [i + 1] == 'q') {
			
				if ((unsigned int) ram [i+2] >= ASCII_0 && 
				      (unsigned int) ram [i+2] <= 57) {
				        return (unsigned int) ram [i+2] - ASCII_0;
				} 
				
				return (unsigned int) ram [i+2] ;
			} else if (ram [i + 1] == 'r') {
			
				if ((int) ram [i + 2] >= ASCII_a) {
					return (unsigned int) ram [(int) (offset - ((int) ram [i + 2] - 71))] ;
					
				} else {
					return (unsigned int) ram [(int) (offset - ((int) ram [i + 2] - ASCII_A))] ;
				}
				
			} else if (ram [i + 1] == 's') {
			
				if ((int) ram [i + 2] >= ASCII_a) {
					return (unsigned int) ram [(unsigned int) ram [(int) (offset - ((int) ram [i + 2] - 71))] ] - ASCII_0;
					
				} else {
					return (unsigned int) ram [(unsigned int) ram [(int) (offset - ((int) ram [i + 2] - ASCII_A))] ]; 
				}
				
			}
	return 0;
}

/*
This function takes the program counter, and the array containing the,
length of each instruction in program code. It mainipulates the data,
and returns the index of the instruction referred to by the program
counter.

parameters:
pc - program counter (register 7) (int)
pcLen - array containing the length of all instructions in the 
program code (integer array)

return:
the index of the instruction referred to by the program
counter (long)
*/
long program_counter_helper (int pc, int* pcLen) {
	long index = 0;
	int j = 0;
	for (j = 0; j < pc; j ++) {
		index += pcLen [j] + 1;
	}
	return index;
}

/*
This function finds the type of values required by a particular
instruction in the program code. It accesses the type of value, and 
value, and stores it in in the stack/ram (for that function).

parametrers:
ram -> ram of the architecture. (unsigned char array)
  registers -> the registers in the architecture. (unsigned char array)
  program_code_final -> the array storing the program code. (char array)
  i -> the position of the particular instruction in program code.
  
  return:
  it returns 0 on success.
*/
int finding_type (unsigned char* ram, unsigned char* registers, 
		    char* program_code_final, int i) {
			
			if (program_code_final [i + 4] == 'R') {
			
					ram [(int) registers [4]] = 'p';
					registers [4] += 1;
					ram [(int) registers [4]] = program_code_final [i + 8];
					registers [4] += 1;
			} else if (program_code_final [i + 4] == 'V') {
			
					ram [(int) registers [4]] = 'q';
					registers [4] += 1;
					ram [(int) registers [4]] = (int) program_code_final [i + 8];
					
					if (program_code_final [i + 9] == ' ' || program_code_final [i + 9] == '\n') {
					} else {
					ram [(int) registers [4]] = ((int) ram [(int) registers [4]] - ASCII_0) * 10 
									+ (int) program_code_final [i + 9] - ASCII_0;
									
						if (program_code_final [i + 10] == ' ' || program_code_final [i + 10] == '\n') {
						   } else {
							ram [(int) registers [4]] = ((int) ram [(int) registers [4]] ) * 10 
											+ (int) program_code_final [i + 10] - ASCII_0 ;
						   }
						   
					}
					registers [4] += 1;
					return ram [(int) registers [4] - 1];
			} else if (program_code_final [i + 4] == 'S') {
			
					ram [(int) registers [4]] = 'r';
					registers [4] += 1;
					ram [(int) registers [4]] = program_code_final [i + 8];
					registers [4] += 1;
			} else if (program_code_final [i + 4] == 'P') {
			
					ram [(int) registers [4]] = 's';
					registers [4] += 1;
					ram [(int) registers [4]] = program_code_final [i + 8];
					registers [4] += 1;
			}
			return 0;
}

/*
This function creates the stack for the particular function defined
by the label. (Execution starts with function label 0)

parameters:
program_code_final -> the array storing the program code. (char array)
ram -> ram of the architecture. (unsigned char array)
registers -> the registers in the architecture. (unsigned char array)
label -> the function label for which, the stack is being created (int)
pcLen -> array containing the length of all instructions in the 
program code (integer array)

return:
it returns 0 on success.

note: when creating stack frame, we use specific characters to denote
opcode, and type.
h->MOV, i->CAL, j->RET, k->REF, l->ADD, m->PRINT, n->NOT, o->EQU
p->REG, q->VAL, r->STK, s->PTR
*/
int create_stack_frame (char* program_code_final, unsigned char* ram,
		 unsigned char* registers, int label, int* pcLen) {
		 
	label = label - ASCII_0;
	long z = 0;
	registers [7] = 0;

	for (z = 0; z < (long) strlen (program_code_final) ; z++) {
	
		if (program_code_final [z] == 'F' && 
			program_code_final [z + 1] == 'U' &&
			 	program_code_final [z + 11] == (char) (label + ASCII_0)) {
			break;
		}
		if (program_code_final [z] == '\n') {
			registers [7] += 1;
		}
	}
	
	for ( ; z < (long) strlen (program_code_final) ; z++) {
	
		if (program_code_final [z] == 'F' && 
			program_code_final [z + 1] == 'U' &&
				 program_code_final [z + 11] == (char) (label + ASCII_0)) {
			while ( 1 > 0) {
			
				registers [7] += 1;
				z = program_counter_helper (registers[7], pcLen);
				if (program_code_final [z + 4] == 'M') {
				
					ram [(int) registers [4]] = 'h';
					registers [4] = (int) registers [4] + 1;
					finding_type (ram, registers, program_code_final, z + 4);
					finding_type (ram, registers, program_code_final, z + 10);
				} else if (program_code_final [z + 4] == 'C') {
				
					ram [(int) registers [4]] = 'i';
					registers [4] = (int) registers[4] + 1;
					registers[6] = finding_type (ram, registers, program_code_final, z + 4);      
				 
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					
				} else if (program_code_final [z + 4] == 'R' 
				&& program_code_final [z + 6] == 'T') {
				
					ram [(int) registers [4]] = 'j';
					registers [4] = (int) registers[4] + 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;

					if (z + 8 < (long) strlen (program_code_final) 
							&& program_code_final [z + 8] == 'F') {
						break;
					} 
					
					if (z + 8 == (long) strlen (program_code_final)) {
						break;
					}
				} else if (program_code_final [z + 4] == 'R') {
				
					ram [(int) registers [4]] = 'k';
					registers [4] = (int) registers[4] + 1;
					finding_type (ram, registers, program_code_final, z + 4);
					finding_type (ram, registers, program_code_final, z + 10);
				} else if (program_code_final [z + 4] == 'A') {
				
					ram [(int) registers [4]] = 'l';
					registers [4] = (int) registers[4] + 1;
					finding_type (ram, registers, program_code_final, z + 4);
					finding_type (ram, registers, program_code_final, z + 10);
				} else if (program_code_final [z + 4] == 'P') {
				
					ram [(int) registers [4]] = 'm';
					registers [4] = (int) registers[4] + 1;
					finding_type (ram, registers, program_code_final, z + 6);
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
				} else if (program_code_final [z + 4] == 'N') {
				
					ram [(int) registers [4]] = 'n';
					registers [4] = (int) registers[4] + 1;
					finding_type (ram, registers, program_code_final, z + 4);
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
				} else if (program_code_final [z + 4] == 'E') {
				
					ram [(int) registers [4]] = 'o';
					registers [4] = (int) registers[4] + 1;
					finding_type (ram, registers, program_code_final, z + 4);
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
					ram [(int) registers [4]] = 'z';
					registers [4] += 1;
				} 
			}
		}
		break;
	}
	return 0;
}


/*
This function executes the stack frame of the current function.
After the return, the stack frame resets.

parameters:
program_code_final -> the array storing the program code. (char array)
ram -> ram of the architecture. (unsigned char array)
registers -> the registers in the architecture. (unsigned char array)
label -> the function label for which, the stack is being created (int)
pcLen -> array containing the length of all instructions in the 
program code (integer array)
offset -> the position in ram, from where the particular stack frame,
started growing from. (unsigned int)
ram_s1 -> position in end of memory, reserved for stack use. (unsigned
 char)
ram_s1 -> position in end of memory, reserved for stack use. (unsigned
 char

return:
it returns 0 on success.
*/
int execute_stack_frame (char* program_code_final, unsigned char* ram,
		 unsigned char* registers, int label, int* pcLen,
		 		 unsigned int offset, unsigned char ram_s1, unsigned char ram_s2) {
		 		 
	label = label - ASCII_0;
	int i = 0;
	registers [7] = 0;
	for (i = (int) registers [5]; i < (int) registers [4]; ) {
	
		if (registers [7] == 0) {
			i = (int) registers [5];
		} else {
			i = (int) registers [5] + registers[7] * 5;
		} 
		
		if (ram[i] == 'h') {
			registers [6] = get_value (i + 2, ram, registers, offset);
			
			if (ram [i + 1] == 'p') {
				registers [(unsigned int)ram[i+2] - ASCII_0] = registers [6];
			} else if (ram [i + 1] == 'r') {
			
				if ((int) ram [i + 2] >= ASCII_a) {
					ram [(int) (offset - ((int) ram [i + 2] - 71))] = registers [6];
					
				} else {
					ram [(int) (offset - ((int) ram [i + 2] - ASCII_A))] = registers [6];
				}
				
			} else if (ram [i + 1] == 's') {
			
				if ((int) ram [i + 2] >= ASCII_a) {
					ram [ram [(int) (offset - ((int) ram [i + 2] - 71))] ] = registers [6];
					
				} else {
					ram [ram [(int) (offset - ((int) ram [i + 2] - ASCII_A))] ] = registers [6];
				}
				
			}
			registers [7] += 1;
		} else if (ram [i] == 'i') {
		
			ram_s2 = registers [5];
			registers [5] = registers [4];
			ram_s1 = registers [7];
			create_stack_frame (program_code_final, ram,
					 registers, (int) ram [i + 2], pcLen);
			unsigned int tempoff = stack_offset(ram);
			execute_stack_frame (program_code_final, ram,
					 registers, (int) ram [i + 2], pcLen, tempoff, ram_s1, ram_s2);
					 
			stack_reset(ram, tempoff);
			
			registers [4] = registers [5];
			registers [7] = ram_s1;
			registers [5] = ram_s2;
			registers [7] += 1;
			
		} else if (ram [i] == 'j') {
			break;
		} else if (ram [i] == 'k') {
		
			if ( ram [i + 3] == 'r') {
			
				if ((int) ram [i + 2] >= ASCII_a) {
					registers [6] = (unsigned int) (offset - ((int) ram [i + 4] - 71)) ;
					
				} else {
					registers [6] = (unsigned int) (offset - ((int) ram [i + 4] - ASCII_A)) ;
				} 
				
			}
			
			if ( ram [i + 3] == 's') {
			
				if ((int) ram [i + 4] >= ASCII_a) {
					registers [6] = (unsigned int) ram [(int) (offset - ((int) ram [i + 4] - 71))] ;
					
				} else {
					registers [6] = (unsigned int) ram [(int) (offset - ((int) ram [i + 4] - ASCII_A))] ; 
				}
				
			}
			
			if ( ram [i + 1] == 'r') {
			
				if ((int) ram [i + 2] >= ASCII_a) {
					ram [(int) (offset - ((int) ram [i + 2] - 71))] = registers [6];	
				} else {
					ram [(int) (offset - ((int) ram [i + 2] - ASCII_A))] = registers [6];	
				}
				
			}
			
			if ( ram [i + 1] == 'p') {
				registers [(unsigned int)ram[i+2] - ASCII_0] = registers [6]; 
			}
			
			if ( ram [i + 1] == 's') {
			
				if ((int) ram [i + 2] >= ASCII_a) {
					ram [(unsigned int) ram [(int) (offset - ((int) ram [i + 2] - 71))] ] = registers [6];	
				} else {
					ram [(unsigned int) ram [(int) (offset - ((int) ram [i + 2] - ASCII_A))] ] = registers [6]; 	
				}
				
			}		
			registers [7] += 1;
		} else if (ram [i] == 'l') {
		
			registers [(unsigned int) ram[i+2] - ASCII_0] = registers [(unsigned int) ram[i+2] - ASCII_0] 
						+ registers [(unsigned int) ram[i+4] - ASCII_0];
			registers [7] += 1;
		} else if (ram [i] == 'm') { 
		
			registers [6] = get_value (i, ram, registers, offset);
			printf("%u\n", (unsigned int) registers [6] );
		        registers [7] += 1;
		} else if (ram [i] == 'n') { 
		
			registers [(unsigned int) ram[i+2] - ASCII_0] = ~registers [(unsigned int) ram[i+2] - ASCII_0];
			registers [7] += 1;
		} else if (ram [i] == 'o') { 
		
			registers [6] = get_value (i, ram, registers, offset);
			
			if (registers [6] == 0) {
				registers [(unsigned int) ram[i+2] - ASCII_0] = 1;
			} else {
				registers [(unsigned int) ram[i+2] - ASCII_0] = 0;
			}
			registers [7] += 1;
		}
	}
	return 0;
}

/*Disassembler for x2017 binaries*/

/*
This function finds the index of the function in program code.

parameters:
program_code : the program code calculated (char array)
func : the function number that we have to find (like 1st function, 2nd
 function etc.) (int)
 
 return:
 If function found - it returns the index of func
 If function not found - it returns 0
*/
int index_of_func (char* program_code, int func) {
	int i = 0;
	int cnt = 0;
	for (i = strlen (program_code) - 1; i > 0; i--) {
		if (program_code [i] == 'F' &&
		     program_code [i + 1] == 'U') {
		     	cnt += 1;
		}
		if (cnt == func) {
			return i;
		}
	}
	return 0;
}

/*
This function takes the list of stack variables in the whole
program code, in the order we encountered it. This function removes
the duplicate values from the list, to ease the process of assigning
stack variables to them. (stack symbols are assigned for each function,
starting from A)

parameters:
stack - list of stack variables in the whole program code, in the order
 we encountered it (int array)
length_stack - the number of elements in stack (int)
new_stack - the stack, in which we will store the variables,removing 
the duplicates. (int array)
size - the original size of the stack array (int)

return:
new_stack_len - it returns the length of new stack (without duplicates) (int)
*/
int removeDuplicates (int* stack, int length_stack, int* new_stack, int size) {
          
    new_stack [0] = stack [0];
    int new_stack_len = 1;
    
    int *temp1;
    temp1 = (int*) malloc (size * sizeof (int));
    int *temp2;
    temp2 = (int*) malloc (size * sizeof (int));
    
    int z;
    for (z = 0; z < size; z++) {
    	temp1 [z] = 3;
    	temp2 [z] = 3;
    }
    
    int k = 0;
    int i = 1;   
    int p,e,q,j,t,n; /*Helper variables*/  
    while (i <= length_stack)
    {   
        if (stack [i] >= 0) {  
            temp1 [k] = stack [i];
            k++;
        } else {  
            e=k-1;
            q=0;
            while (q < k) {
                temp2 [q] = temp1 [e];
                e--;
                q++;
            }
            
            for (p = 0; p < k; p++) {
            	for (j = p + 1; j < k; j++) {
            		if (temp2 [p] == temp2 [j]) {
                		for (t = j; t < k; t++) {
                    			temp2 [t] = temp2 [t + 1];
                		}
               	 	k--;
                	 	j--;
            		}
            	}
            }
      
            n = k - 1;
            while (n >= 0) {
                new_stack [new_stack_len] = temp2 [n];
                n--;
                new_stack_len++;
            }
            new_stack [new_stack_len] = stack [i];
            new_stack_len++;
            k = 0;
        }
        i++;
    }
   
    free (temp1);
    free (temp2);
    return new_stack_len - 1;
}

/*
This function replaces all the occurrences of old word after a given 
index m, with new word in a string in C, and then stores it in resultant
string.

parameters:
string - string whose words we have to replace (char array)
old_word - word which needs to be replaced (char array)
new_word - word which replaces the old word (char array)
m - index after which, we need to replace words (int)

return:
it returns 0 on success.
*/
int replace_word (char* string, char* old_word,
                  char* new_word, int m, char* result) {
    int i, cnt = 0;
    int new_word_len = strlen (new_word);
    int old_word_len = strlen (old_word);
  
    for (i = 0; string [i] != '\0'; i++) {
        if (strstr (&string [i], old_word) == &string [i]) 
        {
            cnt++;
            i += old_word_len - 1;
        }
    }

    i = 0;
    while (*string) 
    {   
        if (strstr (string, old_word) == string && i > m) {
            strcpy (&result[i], new_word);
            i += new_word_len;
            string += old_word_len;
        } else {
            result[i++] = *string++;
        }
    }
  
    result[i] = '\0';
    return 0;
}

/*
This function reverses the string from starting index i to index j.

parameters:
rev - it stores the string with characters between the given indices 
reversed. (char array)
string - the string that needs to be reversed. (char array)
i - starting index (int)
j - ending index (int)

return:
void, it does not returns anything, it just reverses the string, and
 stores the reversed string in rev.
*/
void reverse (char* rev, char* string, int i, int j) {
	int k = 0;
	for (k = i; k <= j; k++) {
		char temp [5];
		sprintf (temp, "%c", string [k]);
		strcat (rev, temp);
	}
}

/*
This function calculates the stack symbols based on their occurrence 
number in the function. Stack symbols go from A-Z then they start with 
lower case alphabets a. 

parameters:
n - the occurrence number of symbol in function (int)

return:
the corresponding stack symbol (int)
*/
char get_stk_ptr (int n) {
	int i = ASCII_A;
	char c;
	if (n < NUM_OF_ALPHABETS) {
		c = i + n;
	}
	else{
		i = ASCII_a;
		c = i + (n - NUM_OF_ALPHABETS);
	}
	return c; 
} 

/*
This function calculates the power of a number.

parameters:
base - the number whose power has to be calculated (double)
exponent - the power that we need to calculate (double)

return:
the result of power function (double)
*/
double own_pow (double base, double exponent) {
	double result = 1;
	double i = 0;
	for (i = exponent; i > 0; i --) {
		result *= base;
	}
	return result;   
}


/*
This function replaces the stack values in the program code for a 
particular function with appropriate stack symbols.

parameters:
func- it checks the stack value for this particular function number (int)
finish - denotes the end of the stack (int)
program_code- the program code (char array)
file_len - the length of binary file (int)
program_code_final - the code after replacing the symbols are stored in
this variable (char array)
len - the length of stack array (int)
stk - stack array that stores the functions followed by their stack value
The stack values are then replaced with particular symbols for the given 
func.(int array)

return:
it returns 0 on success
*/
int replace_stack_symbol (int func, int finish, char* program_code, int file_len,
                 char* program_code_final, int len, int* stk) {

	int i = 0;
	int start;
	int end;
	char c = 'A';
	for (i = 0; i < len; i++) {
		if (stk [i] == func) {
			start = i + 1;
		}
		if (stk [i] == finish && finish < 0) {
			end = i - 1;
		}
	}
	if (finish >= 0) {
		end = finish - 1;
	}
	for (i = end; i >= start; i--) {

		char buf [SMALL_BUF] = {0};
		sprintf (buf, "STK %d ", stk [i]);
		char temp [SMALL_BUF] = {0};
		sprintf (temp, "STK %c ", c);
		int t = index_of_func (program_code, func * -1);
		
		replace_word (program_code, buf, temp, t,   program_code_final);
		
		strcpy (program_code, program_code_final);
		sprintf (buf, "STK %d\n", stk [i]);
		sprintf (temp, "STK %c\n", c);
		replace_word (program_code, buf, temp, t,   program_code_final);
		
		strcpy (program_code, program_code_final);
		sprintf (buf, "PTR %d ", stk [i]);
		sprintf (temp, "PTR %c ", c);
		replace_word (program_code, buf, temp, t,   program_code_final);
		
		strcpy (program_code, program_code_final);
		sprintf (buf, "PTR %d\n", stk [i]);
		sprintf (temp, "PTR %c\n", c);
		replace_word (program_code, buf, temp, t,   program_code_final);
		
		strcpy (program_code, program_code_final);
		
		c += 1;
		if (c > 90) {
			c = ASCII_a + (i - NUM_OF_ALPHABETS);
		}
	}
	return 0;
}


/*
Function to calculate 2's complement of a particular number.

parameter:
number - the number whose 2's complement has to be calculated (int)

return:
the 2's complement result (int)
*/

int twos_complement (int number) {
	char str [BUF];
	sprintf (str, "%d", number);
	int s = strlen (str);
	int num = 0;
	int c;
	int i = 0;
	
	for (i = 0; i < s; i++) {
		c = number % 10;
		num = num + c * own_pow (10, i);
		number = number / 10;
		if (c == 1) {
			break;
		}
	}
	
	for (int p = i + 1; p < 8; p++) {
		if (p < s) {
			c = number % 10;
			if (c == 1) {
				num = num + 0*own_pow (10, p);
			} else {
				num = num + 1*own_pow (10, p);
			} 
		number = number / 10;
		} else {
			num = num + 1*own_pow (10, p);
		}
	}
	return num;
}

/*
This function converts the given binary number to decimal.

parameters:
n - the binary number (int)

return:
the result of binary to decimal conversion (int)
*/
int bin_to_dec (int n) {
	int dec = 0;
	int i = 0;
	int rem = 0;
	while (n != 0) {
		rem = n % 10;
		n = n / 10;
		dec += rem * own_pow (2, i);
		++i;
	}
	return dec;
}

/*
This function counts the number of digits/length of a number.

parameter:
n - the number whose length needs to be found (int)

return:
length of the number (int)
*/
int length (int n) {
	int count = 0;
	while (n != 0) {
		n /= 10;
		++count;
	}
	return count;
}

/*
The function calculates the value of stack/ pointer stored in the binary
 of complete program code between two indices. It just accesses the value
 between those indices, and stores them in an integer.
 
 parameters:
 binary - binary of program code (int array)
 i - starting index (int)
 j - ending index (int)
 
 return:
 the final result
*/
int get_value_stk_ptr (int* binary, int i, int j) {
	int k = 0;
	int bin = 0;
	for (k = j; k <= i; k++) {
		bin = bin * 10 + binary [k];
	}
	return bin;
}

/*
This function converts the given decimal number to binary.

parameters:
n - the decimal number (int)

return:
the result of decimal to binary conversion (int)
*/
int dec_to_binary (int n) {
	int r = 0;
	int binary = 0;
	int d = n;
	int temp = 1;
	while (n != 0) {
		r = n % 2;
		n = n / 2;
		binary = binary + r * temp;
		temp = temp * 10;
	}
	if (d < 0) {
		binary = twos_complement (binary * (-1));
   	}
	return binary;
}

/*
The function calculates the value of register/ value type stored in the
 binary of complete program code between two indices. It just accesses the
 value between those indices, and stores them in an integer.
 
 parameters:
 binary - binary of program code (int array)
 i - starting index (int)
 j - ending index (int)
 
 return:
 the final result
*/
int get_value_reg (int* binary, int i, int j) {
	int k = 0;
	int bin = 0;
	for (k = j; k <= i; k++) {
		bin = bin * 10 + binary [k];
	}
	int num = bin_to_dec (bin);
	return num;
}

/*
This function finds the type of value the binary values correspond to. It
 then finds the corresponding value, register, stack, pointer value and
  adds it to the program line. These program lines, further add up to form
   the program code.

parameters:
binary -> binary of program code. (int array)
program_line -> each line of program code. (char array)
value_reg_buffer -> buffer to store the value/ register value. (char array)
j -> the index of binary, which we are accessing. (int)
cstk -> the counter telling the position of next element to be entered in stk. (int)
stk -> stack array that stores the functions followed by their stack value
The stack values are replaced with particular symbols for the given 
func in later stage of code. (int array)
result -> the reult which stores the updated value of j in index 0, and
 updated value of cstk in index 1. (int array)
type -> the type 0 tells that the value is the first variable of a
 operation, which needs to be followed by another value. Hence we add a
  space after the value. Type 1 tells it is the second or, only variable
   of the operation, and hence we add a new line character to the end.
    This step helps maintain the correct format of code. (int)

return:
this function has return type void. It just accesses the values from
 binary code and adds them to program line. 
*/
void calculating_program_line (int* binary, char* program_line, 
                  char* value_reg_buffer, int j, int cstk, int *stk, 
                  int* result, int type) {
					if (binary[j] == 0 && 
	  				binary [j-1] == 0) {
	  					strcat(program_line, "VAL ");
	  					j = j - 2;
	  					int value_reg = get_value_reg (binary, j , j - 7);
	  					
	  					if (type == 0) {
	  						sprintf (value_reg_buffer, "%d ", value_reg);
	  					} else {
	  						sprintf (value_reg_buffer, "%d\n", value_reg);
	  					}
	  					
	  					strcat(program_line, value_reg_buffer);
	  					j = j - 8;
	  				} else if (binary[j] == 1 && 
	  					binary [j-1] == 0) {
	 						strcat(program_line, "REG ");
	  						j = j - 2;
	  						int value_reg = get_value_reg (binary, j, j - 2);
	  						
	  						if (type == 0) {
	  						sprintf (value_reg_buffer, "%d ", value_reg);
	  					} else {
	  						sprintf (value_reg_buffer, "%d\n", value_reg);
	  					}
	  					
	  						strcat(program_line, value_reg_buffer);
	  						j = j - 3;
	  				} else if (binary[j] == 0 && 
	  				binary [j-1] == 1) {
	  
	  					strcat(program_line, "STK ");
	  					j = j - 2;
	  					int value_reg = get_value_stk_ptr (binary, j, j - 4);
	  					
	  					if (type == 0) {
	  						sprintf (value_reg_buffer, "%d ", value_reg);
	  					} else {
	  						sprintf (value_reg_buffer, "%d\n", value_reg);
	  					}
	  					
	  					strcat(program_line, value_reg_buffer);
	  					stk [cstk] = value_reg;
	  					cstk += 1;
	  					j = j - 5;
	  				} else if (binary[j] == 1 && 
	  				binary [j-1] == 1) {
	  					strcat(program_line, "PTR ");
	  					j = j - 2;
	  					int value_reg = get_value_stk_ptr (binary, j , j - 4);
	  					
	  					if (type == 0) {
	  						sprintf (value_reg_buffer, "%d ", value_reg);
	  					} else {
	  						sprintf (value_reg_buffer, "%d\n", value_reg);
	  					}
	  					
	  					strcat(program_line, value_reg_buffer);
	  					stk [cstk] = value_reg;
	  					cstk += 1;
	  					j = j - 5;
	  				} else {
	  					perror ("Binary does not corresponds to any value/ register/ stack/ pointer. Check the input file. \n");
	  					exit (1);
	  				}
	  				result [0] = j;
	  				result [1] = cstk;
}


/* Main function
It calls the appropriate functions, to convert binary file to program
code.
It then executes the program code, by calling appropriate functions to
create stack for each function, execute it, and then reset the stack 
space.

parameter:
argc - number of command line arguments (int)
argv - the array of addresses of command line arguments (array of
 pointers to char)

return:
it returns 0 on success
*/
int main (int argc, char** argv) {

	FILE *file_ptr;
	char *buffer;
	int *binary;
	long file_len;
	int i;
	
	if (argc < 2) {
	    perror ("Too less arguments. \n Give file name as command line argument. \n");
	    exit (1);
	}

	file_ptr = fopen (argv[1], "rb");
	if (file_ptr == NULL) {
		perror("File could not be opened. \n");
		exit (1);
	}
	
	int success = fseek (file_ptr, 0L, SEEK_END);
	if (success != 0) {
		perror("fseek unsuccessful. \n");
		exit (1);
	}
	
	file_len = ftell (file_ptr);
	if (file_len < 0) {
		perror("ftell unsuccessful. \n");
		exit (1);
	}
	
	rewind (file_ptr);
	buffer = (char *) malloc ((file_len + 1) * sizeof (char));
	binary = (int *) malloc ((file_len + 1) * sizeof (int) * 8);
	
	// reading, and storing the contents of file in buffer.
	for (i = 0; i < file_len; i++) {
		fread (buffer + i, 1, 1, file_ptr);
	}
	buffer [i] = '\0';

	int counter = 0;
	int j, k;
	
	// converting the sets of 8 indexes (8 bits = 1 byte) in buffer
	// to binary.
	for (i = 0; i < file_len; i++) {
		int b = dec_to_binary (buffer [i]);
		int len = length (b);

		for (j = 0; j < 8 - len; j++) {
			binary [counter] = 0;
			counter += 1;
		}
		
		for (k = len - 1; k >= 0; k--) {
			int temp = b;
			temp = temp / own_pow (10, k);
			temp = temp % 10;
			binary [counter] =  temp;
			counter += 1;
		}
	}
	

	int cnt = 0;
	int num_inst = 0;
	char value_reg_buffer [SMALL_BUF] = {0};
	char *program_code_rev; // stores the program code in reverse
	program_code_rev = (char*) calloc (file_len * BUF, 1);
	int pcrev = 0;

	char *program_code; //stores program code without updated stack symbols
	program_code = (char*) calloc (file_len * BUF, 1);

	char *program_code_final; // stores final program code
	program_code_final = (char*) calloc (file_len * BUF, 1);

	int *lenrev; // stores length of each instruction in program_code_rev
	lenrev = (int*) malloc (file_len * sizeof (int));
	int clen = 0;

	int *stk; // stores the binary representation of stack symbols.
	stk = (int*) malloc (file_len * 5 * sizeof (int));
	int cstk = 0;

	int fno = -1;
	
	cnt = 0;
	num_inst = 0;
	for (j = file_len * 8 - 1; j >= 0; ) {
		if (j <= 7) {
			j = j - 1;
			
		} else if (cnt < 5) {
	 		num_inst = num_inst + binary [j] *own_pow (10, cnt);
	 		j = j - 1;
	 		cnt += 1;
	 		
	 	} else if (cnt == 5) {
	 	
	  		num_inst = bin_to_dec (num_inst);
	  		stk [cstk] = fno;
	  		cstk += 1;
	  		fno = fno - 1;
	  		
	  		// while loop runs the number of instructions times
	  		// it calculates all the instructions in the function
	  		while (num_inst > 0) {
	  
	  			if ( binary[j] == 0 && 
	  			binary [j-1] == 0 && 
	  			binary [j-2] == 0) {
	  			
	  				char program_line[BUF] = "    MOV ";
	  				j = j - 3;
	  				int result [2] = {0,0};
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 0);
	  						
	  				j = result [0];
	  				cstk = result [1];
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 1);
	  						
	  				j = result [0];
	  				cstk = result [1];
	  				 
	  				strcat (program_code_rev, program_line);
	  				pcrev += strlen (program_line);
	  				lenrev [clen] = strlen (program_line);
	  				clen += 1;
	  			} else if (binary[j] == 1 && 
	  					binary [j-1] == 0 && 
	  					binary [j-2] == 0) {
	  				char program_line [BUF] = "    CAL ";
	  				j = j - 3;
	  				
	  				int result [2] = {0,0};
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 1);
	  						
	  				j = result [0];
	  				cstk = result [1];
	  				
				  	strcat (program_code_rev, program_line);
					pcrev += strlen(program_line);
					lenrev [clen] = strlen (program_line);
					clen += 1;
	  			} else if (binary[j] == 0 && 
	  			    binary [j-1] == 1 && 
	  			    binary [j-2] == 0) {
	  			    
	  				char program_line[BUF] = "    RET\n";
				  	j = j - 3;
				  	strcat (program_code_rev,program_line);
				  	pcrev += strlen (program_line);
				  	lenrev [clen] = strlen (program_line);
				  	clen += 1;
	  			} else if (binary[j] == 1 && 
	  			     binary [j-1] == 1 && 
	  			     binary [j-2] == 0) {
	  			     
	  				char program_line [BUF] = "    REF ";
	  				j = j - 3;
	  				int result [2] = {0,0};
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 0);
	  						
	  				j = result [0];
	  				cstk = result [1];
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 1);
	  						
	  				j = result [0];
	  				cstk = result [1];	  
				  	strcat (program_code_rev, program_line);
				  	pcrev += strlen (program_line);
				  	lenrev [clen] = strlen (program_line);
				  	clen += 1;
	  			} else if (binary[j] == 0 && 
	  			     binary [j-1] == 0 && 
	  			     binary [j-2] == 1) {
	  			     
	  				char program_line[BUF] = "    ADD ";
	  				j = j - 3;
	  				int result [2] = {0,0};
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 0);
	  						
	  				j = result [0];
	  				cstk = result [1];
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 1);
	  						
	  				j = result [0];
	  				cstk = result [1];	
				  	strcat (program_code_rev, program_line);
				  	pcrev += strlen (program_line);
				  	lenrev [clen] = strlen (program_line);
				  	clen += 1;
	  			} else if (binary[j] == 1 && 
	  			   binary [j-1] == 0 && 
	  			   binary [j-2] == 1) {
	  				char program_line [BUF] = "    PRINT ";
	  				j = j - 3;
	  				int result [2] = {0,0};
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 1);
	  						
	  				j = result [0];
	  				cstk = result [1];
				  	strcat (program_code_rev, program_line);
				  	
				  	pcrev += strlen (program_line);
				  	lenrev [clen] = strlen (program_line);
				  	clen += 1;
	  			} else if (binary[j] == 0 && 
	  			    binary [j-1] == 1 && 
	  			    binary [j-2] == 1) {
	  				char program_line [BUF] = "    NOT ";
	  				j = j - 3;
	  				int result [2] = {0,0};
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 1);
	  						
	  				j = result [0];
	  				cstk = result [1];
				  	strcat (program_code_rev, program_line);
				  	pcrev += strlen (program_line);
				  	lenrev [clen] = strlen (program_line);
				  	clen += 1;
	  			} else if (binary[j] == 1 && 
	  			   binary [j - 1] == 1 && 
	  			   binary [j - 2] == 1) {
	  				char program_line [BUF] = "    EQU ";
	  				j = j - 3;
	  				int result [2] = {0,0};
	  				calculating_program_line (binary, program_line, value_reg_buffer, 
	  						j, cstk, stk, result, 1);
	  						
	  				j = result [0];
	  				cstk = result [1];
				        strcat (program_code_rev, program_line);
				  	pcrev += strlen (program_line);
				  	lenrev [clen] = strlen (program_line);
				  	clen += 1;
	  			} else {
	  				perror ("Binary does not corresponds to any opcode. Check the input file. \n");
	  				exit (1);
	  			}
	  			num_inst = num_inst - 1;
	  		}
	  		
		  	char program_line [BUF] = "FUNC LABEL ";
		  	int value_reg = get_value_reg (binary, j, j - 2);
		  	
		  	sprintf (value_reg_buffer, "%d\n", value_reg);
		  	strcat (program_line, value_reg_buffer);
		  	strcat (program_code_rev, program_line);
		  	pcrev += strlen (program_line);
		  	lenrev [clen] = strlen (program_line);
		  	clen += 1;
		  	j = j - 3;
		  	cnt = 0;
	 	}
	}
	

	int *nstk;
	nstk = (int*) malloc (file_len * 5 * sizeof (int));
	cstk = removeDuplicates (stk, cstk, nstk, file_len * 5);

	int temp = pcrev;
	for (i = clen - 1; i >= 0; i--) {
		reverse (program_code, program_code_rev, temp - lenrev [i], temp - 1);
		temp =  temp - lenrev [i];
	}

	strcpy (program_code_final, program_code);
	
	int *func_starts; // stores the indexes at which new funtion starts.
	// we need this because we need to reset stack values for every function.
	func_starts = (int*) malloc (file_len * sizeof (int));
	int funcp = 0;
	
	for (i = 0; i < pcrev - 4; i++) {
		if (program_code [i] == 'F' && program_code[i+1] == 'U') {
			func_starts [funcp] = i;
			funcp += 1;
		}
	}
	
	// replacing stack symbol binaries with particular stack symbol
	// this function does this for every function
	int numFuncs = (fno + 1) * -1;
	int func = -1;
	for (i = numFuncs - 1; i >= 0; i--) {
		int nextfunc = (func - 1) * -1;
		
		if (nextfunc <= numFuncs) {
			replace_stack_symbol (func, nextfunc * -1, program_code,
			 file_len, program_code_final, cstk, nstk);
		} else {
			replace_stack_symbol (func, cstk, program_code, file_len,
			 program_code_final, cstk, nstk);
		}
		
		func -= 1;
	}

	int length = strlen (program_code_final);
	int *pcLen;
	pcLen = (int*) malloc (file_len * sizeof(int));
	int pc = 0;
	
	int len = 0;
	for (i = 0; i < length; i ++) {
	  if (program_code_final [i] != '\n') {
	  	len += 1;
	  } else {
	  	pcLen [pc] = len;
	  	pc += 1;
	  	len = 0;
	  }
	}
	
	
	/*ONLY USE RAM, REGISTERS AND PROGRAM CODE (program_code_final and pcLen) BELOW*/
	/*NO INFORMATION ABOUT STATE OF MACHINE OUTSIDE RAM AND REGISTERS*/
	
	unsigned char ram [RAM_ARRAY_SIZE];
	unsigned char ram_s1 = ' ';
	unsigned char ram_s2 = ' ';
	unsigned char registers [NUMBER_OF_REGISTERS];
	registers [7] = 0; /*PROGRAM COUNTER*/
	registers [5] = 0; /* STACK FRAME START*/
	registers [4] = 0; /* RAM POINTER (STACK FRAME END)*/
	/*register 6 is for temporary use*/
	
	for (i = 0; i < RAM_ARRAY_SIZE; i ++) {
		ram [i] = 'w';
	}
	
	unsigned int offset = stack_offset (ram);

	create_stack_frame (program_code_final, ram, registers, ASCII_0, pcLen);
	execute_stack_frame (program_code_final, ram, registers, ASCII_0,
				 pcLen, offset, ram_s1, ram_s2);
	
	
	free(pcLen);
	free(program_code_rev);
	free(program_code_final);
	free(program_code);
	free(func_starts);
	free(lenrev);
	free(buffer);
	free(binary);
	free(stk);
	free(nstk);
	fclose(file_ptr);
	
	return 0;
}





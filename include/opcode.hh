typedef unsigned char opcode_t;


// stack operations are always of the form 0x0n-0x3n
//   (enough space for massive expansion if need be)
#define OP_NOP           0x00
#define OP_PUSH_NIL      0x01
#define OP_PUSH_NUM      0x02
#define OP_PUSH_IDENT    0x03
#define OP_PUSH_STR      0x04
#define OP_PUSH_LIST     0x05
#define OP_SKIP          0x06
// look up an identifier and push it's value to the stack
#define OP_PUSH_LOOKUP   0x07
// store the value at the top of the stack in the current env
#define OP_STORE_GLOBAL  0x08
#define OP_PUSH_RAW      0x09
#define OP_STORE_LOCAL   0x0a

// math operations are always of the form 0x5n
#define OP_ADD        0x50
#define OP_SUB        0x51
#define OP_MUL        0x52
#define OP_DIV        0x53
#define OP_MOD        0x54
#define OP_LT         0x56

// logic operations are always of the form 0x6n
#define OP_AND        0x60
#define OP_OR         0x61
#define OP_NOT        0x62


// An OP_CALL will create a new call stack, requiring
#define OP_CALL       0x70
// take the two top values on the stack and make a list of them
// for example, the construction of (1 2) is as follows:
// push nil
// push 2
// cons
// push 1
// cons
#define OP_CONS       0x71
#define OP_PRINT_     0x72
#define OP_EXIT       0x73
#define OP_CAR        0x74
#define OP_CDR        0x75
// compare the top two values on the stack
#define OP_EQUAL      0x77
#define OP_INTERN     0x78
#define OP_RETURN     0x79
// syscall takes over the need for more basic opcodes.
// it uses the integer value in the instruction to store
// how many arguments were passed to it
#define OP_SYSCALL    0x7b


// jump operations take an absolue pc in the "whole" field
// and when it calls, it will set pc to the location
#define OP_JUMP       0x7c
#define OP_JUMP_FALSE 0x7d

#define OP_BC_RETURN  0x7e

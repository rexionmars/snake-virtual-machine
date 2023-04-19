#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define VM_STACK_CAPACITY 1024

typedef enum {
    TRAP_OK = 0,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_ILLEGAL_INSTRUCTION,
} Trap;

typedef int64_t Word;

typedef struct {
    Word stack[VM_STACK_CAPACITY];
    size_t stack_size;
} Vm;

typedef enum {
    INSTRUCTION_PUSH,
    INSTRUCTION_PLUS
} Instruction_type;

typedef struct {
    Instruction_type type;
    Word operand;
} Instruction;

Vm vm ={0};

Trap vm_execute_inst(Vm *vm, Instruction instruction)
{
    switch (instruction.type) {
        case INSTRUCTION_PUSH:
            if (vm -> stack_size >= VM_STACK_CAPACITY) {
                return TRAP_STACK_OVERFLOW;
            }
            vm -> stack[vm -> stack_size++] = instruction.operand;
            break;

        case INSTRUCTION_PLUS:
            if (vm -> stack_size < 2) {
                return TRAP_STACK_UNDERFLOW;
            }

            vm -> stack[vm -> stack_size - 2] += vm -> stack[vm -> stack_size -1];
            vm -> stack_size -= 1;
            break;

        default:
            return TRAP_ILLEGAL_INSTRUCTION;
    }
    return TRAP_OK;
}

int main()
{
    printf("Hello world!");
    return 0;
}

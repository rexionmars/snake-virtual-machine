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

Instruction instruction_push(Word operand)
{
    return (Instruction) {
        .type = INSTRUCTION_PUSH,
        .operand = operand,
    };
}

Instruction instruction_plus()
{
    return (Instruction) {.type = INSTRUCTION_PLUS};
}

Trap vm_execute_instruction(Vm *vm, Instruction instruction)
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

void vm_dump(const Vm *vm)
{
    printf("Stack:\n");
    if (vm -> stack_size > 0) {
        for (size_t i = 0; i < vm -> stack_size; i++) {
            printf("\033[0;33m-->\033[0m %ld\n", vm -> stack[i]);
        }
    }
    else {
        printf("\033[0;31m-->\033[0m [Empty]\n");
    }
}

Vm vm ={0};

int main()
{
    vm_dump(&vm);
    vm_execute_instruction(&vm, instruction_push(69));
    vm_dump(&vm);
    vm_execute_instruction(&vm, instruction_push(420));
    vm_dump(&vm);
    vm_execute_instruction(&vm, instruction_plus());
    vm_dump(&vm);
    return 0;
}

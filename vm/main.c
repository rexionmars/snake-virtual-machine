#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define VM_STACK_CAPACITY 1024

typedef enum {
    ERROR_OK = 0,
    ERROR_STACK_OVERFLOW,
    ERROR_STACK_UNDERFLOW,
    ERROR_ILLEGAL_INSTRUCTION,
} Error;

const char *error_as_cstr(Error error)
{
    switch (error) {
        case ERROR_OK:
            return "ERROR_OK";
        case ERROR_STACK_OVERFLOW:
            return "ERROR_STACK_OVERFLOW";
        case ERROR_STACK_UNDERFLOW:
            return "ERROR_STACK_UNDERFLOW";
        case ERROR_ILLEGAL_INSTRUCTION:
            return "ERROR_ILLEGAL_INSTRUCTION";
        default:
            assert(0 && "error_as_cstr --> Unreachable");
    }
}

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

#define MAKE_INSTRUCTION_PUSH(value) ((Instruction) {.type = INSTRUCTION_PUSH, .operand = (value)})
#define MAKE_INSTRUCTION_PLUS ((Instruction) {.type = INSTRUCTION_PLUS})



Error vm_execute_instruction(Vm *vm, Instruction instruction)
{
    switch (instruction.type) {
        case INSTRUCTION_PUSH:
            if (vm -> stack_size >= VM_STACK_CAPACITY) {
                return ERROR_STACK_OVERFLOW;
            }
            vm -> stack[vm -> stack_size++] = instruction.operand;
            break;

        case INSTRUCTION_PLUS:
            if (vm -> stack_size < 2) {
                return ERROR_STACK_UNDERFLOW;
            }

            vm -> stack[vm -> stack_size - 2] += vm -> stack[vm -> stack_size -1];
            vm -> stack_size -= 1;
            break;

        default:
            return ERROR_ILLEGAL_INSTRUCTION;
    }
    return ERROR_OK;
}

void vm_dump(FILE *stream, const Vm *vm)
{
    fprintf(stream, "Stack:\n");
    if (vm -> stack_size > 0) {
        for (size_t i = 0; i < vm -> stack_size; i++) {
            fprintf(stream, "--> %ld\n", vm -> stack[i]);
        }
    }
    else {
        fprintf(stream, "--> [Empty]\n");
    }
}

#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof((xs)[0]))

Vm vm ={0};
Instruction program[] = {
    MAKE_INSTRUCTION_PUSH(69),
    MAKE_INSTRUCTION_PUSH(420),
    MAKE_INSTRUCTION_PLUS,
};

int main()
{
    vm_dump(stdout, &vm);

    for (size_t i = 0; i < ARRAY_SIZE(program); ++i) {
        Error error = vm_execute_instruction(&vm, program[i]);

        if (error != ERROR_OK) {
            fprintf(stderr, "ERROR activated --> %s\n", error_as_cstr(error));
            vm_dump(stderr, &vm);
            exit(1);
        }
    }
    vm_dump(stdout, &vm);

    return 0;
}

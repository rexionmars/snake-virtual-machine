#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define red "\033[0;31m"
#define off "\033[0m"

#define VM_STACK_CAPACITY 1024

typedef enum {
    ERROR_OK = 0,
    ERROR_STACK_OVERFLOW,
    ERROR_STACK_UNDERFLOW,
    ERROR_ILLEGAL_INSTRUCTION,
    ERROR_DIVISION_BY_ZERO,
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
        case ERROR_DIVISION_BY_ZERO:
            return "ERROR_DIVISION_BY_ZERO";
        default:
            assert(0 && "error_as_cstr: Unreachable");
    }
}

typedef int64_t Word;

typedef struct {
    Word stack[VM_STACK_CAPACITY];
    size_t stack_size;
    Word ip;
    int halt;
} Vm;

typedef enum {
    INSTRUCTION_PUSH,
    INSTRUCTION_PLUS,
    INSTRUCTION_MINUS,
    INSTRUCTION_MULT,
    INSTRUCTION_DIV,
    INSTRUCTION_JUMP,
    INSTRUCTION_HALT,
} Instruction_type;

typedef struct {
    Instruction_type type;
    Word operand;
} Instruction;

const char *instruction_type_as_cstr(Instruction_type type)
{
    switch(type) {
        case INSTRUCTION_PUSH: return "INSTRUCTION_PUSH";
        case INSTRUCTION_PLUS: return "INSTRUCTION_PLUS";
        case INSTRUCTION_MINUS: return "INSTRUCTION_MINUS";
        case INSTRUCTION_MULT: return "INSTRUCTION_MULT";
        case INSTRUCTION_DIV: return "INSTRUCTION_DIV";
        default: assert(0 && "instruction_type_as_cstr: Unreachable");
    }
}

#define MAKE_INSTRUCTION_PUSH(value) {.type = INSTRUCTION_PUSH, .operand = (value)}
#define MAKE_INSTRUCTION_PLUS {.type = INSTRUCTION_PLUS}
#define MAKE_INSTRUCTION_MINUS  {.type = INSTRUCTION_MINUS}
#define MAKE_INSTRUCTION_MULT  {.type = INSTRUCTION_MULT}
#define MAKE_INSTRUCTION_DIV  {.type = INSTRUCTION_DIV}
#define MAKE_INSTRUCTION_JUMP(addr) {.type = INSTRUCTION_JUMP, .operand = (addr)}

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
            vm -> stack[vm -> stack_size - 2] += vm -> stack[vm -> stack_size - 1];
            vm -> stack_size -= 1;
            break;

        case INSTRUCTION_MINUS:
            if (vm -> stack_size < 2) {
                return ERROR_STACK_UNDERFLOW;
            }
            vm -> stack[vm -> stack_size - 2] -= vm -> stack[vm -> stack_size - 1];
            vm -> stack_size -= 1;
            break;

        case INSTRUCTION_MULT:
            if (vm -> stack_size < 2) {
                return ERROR_STACK_UNDERFLOW;
            }
            vm -> stack[vm -> stack_size - 2] *= vm -> stack[vm -> stack_size - 1];
            vm -> stack_size -= 1;
            break;

        case INSTRUCTION_DIV:
            if (vm -> stack_size < 2) {
                return ERROR_STACK_UNDERFLOW;
            }

            if (vm -> stack[vm -> stack_size -1] == 0) {
                return ERROR_DIVISION_BY_ZERO;
            }
            
            vm -> stack[vm -> stack_size - 2] /= vm -> stack[vm -> stack_size - 1];
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
            fprintf(stream, "   %s%ld%s\n", red, vm -> stack[i], off);
        }
    }
    else {
        fprintf(stream, "   [Empty]\n");
    }
}

#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof((xs)[0]))

Vm vm ={0};

// hardcoded program
Instruction program[] = {
    MAKE_INSTRUCTION_PUSH(69),
    MAKE_INSTRUCTION_PUSH(420),
    MAKE_INSTRUCTION_PLUS,
    MAKE_INSTRUCTION_PUSH(42),
    MAKE_INSTRUCTION_MINUS,
    MAKE_INSTRUCTION_PUSH(2),
    MAKE_INSTRUCTION_MULT,
    MAKE_INSTRUCTION_PUSH(4),
    MAKE_INSTRUCTION_DIV,
};

int main()
{
    vm_dump(stdout, &vm);

    while (!vm.halt) {
        printf("[%s]\n", instruction_type_as_cstr(program[vm.ip].type));
        Error error = vm_execute_instruction(&vm, program[vm.ip]);
        vm_dump(stdout, &vm);

        if (error != ERROR_OK) {
            fprintf(stderr, "TRAP ERROR: %s\n", error_as_cstr(error));
            exit(1);
        }
    }

    return 0;
}

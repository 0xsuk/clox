#ifndef clox_vm_h
#define clox_vm_h

#define STACK_MAX 256 // for now its fixed size

#include "chunk.h"

typedef struct {
  Chunk *chunk;
  uint8_t *ip; // pointer to the next instruction
  Value stack[STACK_MAX];
  Value *stackTop; // points to the just *past* the array. For empty array,
                   // stackTop points at element zero
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpreterResult;

void initVM();
void freeVM();
InterpreterResult interpret(Chunk *chunk);
void push(Value value);
Value pop();

#endif

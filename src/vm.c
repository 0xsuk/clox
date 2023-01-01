#include "vm.h"
#include "debug.h"
#include <stdio.h>

VM vm;

static void resetStack() { vm.stackTop = vm.stack; }

void initVM() { resetStack(); }

void freeVM() {}

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

static InterpreterResult run() {
#define READ_BYTE()                                                            \
  (*vm.ip++) // increment, and return current code, while ip pointing to the
             // next
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    // show every content of the stack
    for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(
        vm.chunk,
        (int)(vm.ip -
              vm.chunk->code)); // the offset of current code is provided
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(constant);
      break;
    }
    case OP_NEGATE:
      push(-pop());
      break;
    case OP_RETURN:
      printValue(pop()); // just for now
      printf("\n");
      return INTERPRET_OK; // return for now
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpreterResult interpret(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

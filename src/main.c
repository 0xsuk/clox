#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int args, const char *argv[]) {
  Chunk chunk;
  initChunk(&chunk);

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123); // constant is offset of constant stored in
                                     // ValueArray(chunk->constants)

  writeChunk(&chunk, OP_RETURN, 123);
  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}

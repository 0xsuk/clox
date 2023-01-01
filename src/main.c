#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

static void repl() {
  char line[1024];
  for (;;) {
    printf("> ");
    if (!fgets(line, sizeof(line), stdin)) { // TODO: ?
      printf("\n");
      break;
    }
  }
}

static char *readFile(const char *path) {
  FILE *file = fopen(path, "rb"); // TODO
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\"\n", path);
    exit(74);
  }
  fseek(file, 0L, SEEK_END);     // TODO: ?
  size_t fileSize = ftell(file); // TODO
  rewind(file);

  char *buffer = (char *)malloc(fileSize + 1); // TODO
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file); // TODO
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytesRead] = '\0';

  fclose(file);  // TODO
  return buffer; // TODO
}

static void runFile(const char *path) {
  char *source = readFile(path);
  InterpreterResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR)
    exit(65);
  if (result == INTERPRET_RUNTIME_ERROR)
    exit(70);
}

int main(int argc, const char *argv[]) {
  initVM();

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: clox [path]\n");
    exit(64);
  }

  freeVM();
  return 0;
}


typedef struct {
  const char *start;
  const char *current;
  int line;
} Scanner;

Scanner scanner;

void initScaner(const char *source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

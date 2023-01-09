#ifndef clox_object_h
#define clox_object_h

#include "chunk.h"
#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION);
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
  OBJ_FUNCTION,
  OBJ_STRING,
} ObjType;

struct Obj {
  ObjType type;
  struct Obj *next;
};

typedef struct {
  Obj obj;     // inheritance
  int arity;   // number of param
  Chunk chunk; // we store each function a different chunk;
  ObjString *name;
} ObjFunction;

struct ObjString {
  Obj obj;
  int length;
  char *chars;
  uint32_t hash;
};

ObjFunction *newFunction();
ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);

void printObject(Value value);

// arguments passed into inline is evaluated only ONCE during compilation, while
// macro isn't
// TODO what the fuck does static inline mean in header file?
static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif

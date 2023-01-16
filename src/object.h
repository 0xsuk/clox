#ifndef clox_object_h
#define clox_object_h

#include "chunk.h"
#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)

#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value))->function)
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
  OBJ_FUNCTION,
  OBJ_CLOSURE,
  OBJ_STRING,
  OBJ_UPVALUE,
  OBJ_NATIVE,
} ObjType;

struct Obj {
  ObjType type;
  bool isMarked;
  struct Obj *next;
};

typedef struct {
  Obj obj;   // inheritance
  int arity; // number of param
  int upvalueCount;
  Chunk chunk; // we store each function a different chunk;
  ObjString *name;
} ObjFunction;

typedef Value (*NativeFn)(int argCount, Value *args);

typedef struct {
  Obj obj;
  NativeFn function;
} ObjNative;

struct ObjString {
  Obj obj;
  int length;
  char *chars;
  uint32_t hash;
};

typedef struct ObjUpvalue {
  Obj obj;
  Value *location;
  Value closed;
  struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
  Obj obj;
  ObjFunction *function;
  ObjUpvalue **upvalues;
  int upvalueCount; // function also stores count, but GC also needs ObjClosure
                    // to hold one
} ObjClosure;       // every function is wrapped by ObjClosure

ObjClosure *newClosure(ObjFunction *function);
ObjFunction *newFunction();
ObjNative *newNavtive(NativeFn function);
ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
ObjUpvalue *newUpvalue(Value *slot);
void printObject(Value value);

// arguments passed into inline is evaluated only ONCE during compilation, while
// macro isn't
// TODO what the fuck does static inline mean in header file?
static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif

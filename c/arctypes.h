#ifndef ARCTYPES_H
#define ARCTYPES_H
#include <stdint.h>
#include <stddef.h>
void * addressofDynamic(const char * name);
size_t sizeofDynamic(const char * name);
size_t offsetofDynamic(const char * typeName, const char * memberName);
#endif// ARCTYPES_H
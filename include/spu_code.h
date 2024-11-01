#ifndef SPU_CODE_H
#define SPU_CODE_H

#include "spu_return_codes.h"
#include "spu_text_reader.h"
#include <stdlib.h>

const int CAPACITY_CHUNK = 100;

struct SpuCode_t {
  int *buffer;

  size_t bufSize;
  size_t bufCapacity;
};


SpuReturnCode  spu_code_compile  (const char *filename, SpuCode_t *code);

SpuReturnCode  spu_code_save     (const SpuCode_t *code, const char *filename);
SpuReturnCode  spu_code_load     (const char *filename, SpuCode_t *code);

SpuReturnCode  spu_code_append   (SpuCode_t *code, int value);

SpuReturnCode  spu_code_allocate (SpuCode_t *code, size_t newCapacity);
SpuReturnCode  spu_code_free     (SpuCode_t *code);


#endif // SPU_CODE_H

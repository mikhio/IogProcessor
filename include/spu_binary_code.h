#ifndef SPU_BINARY_CODE_H
#define SPU_BINARY_CODE_H

#include "spu_return_codes.h"
#include "spu_cmd_ids.h"
#include "iog_stack.h"

struct SpuBin_t {
  IogStack_t stack;
  int *code;
  size_t ip;
  size_t size;
};

SpuReturnCode spu_bin_load     (const char *filename, SpuBin_t *binCode);

SpuReturnCode spu_bin_run      (SpuBin_t *binCode);
SpuReturnCode spu_bin_read_cmd (SpuBin_t *binCode, SpuCmdId_t *curCmd, int *value);

SpuReturnCode spu_bin_free     (SpuBin_t *binCode);

#endif // SPU_BINARY_CODE_H

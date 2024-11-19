#ifndef SPU_BINARY_CODE_H
#define SPU_BINARY_CODE_H

#include "spu_code.h"
#include "spu_return_codes.h"
#include "spu_cmd_ids.h"

#include "iog_stack.h"

const size_t SPU_REGS_NUM = 30;
const size_t SPU_RAM_SIZE = 100000;

struct Spu_t {
  IogStack_t stack;
  IogStack_t callStk;
  SpuCode_t  code;

  size_t     ip;
  int        regs[SPU_REGS_NUM];
  int        ram[SPU_RAM_SIZE];
};


SpuReturnCode spu_run      (Spu_t *proc);
SpuReturnCode spu_read_cmd (Spu_t *proc, cmd_code_t *curCmd, int *value);

SpuReturnCode spu_free     (Spu_t *proc);

#endif // SPU_BINARY_CODE_H

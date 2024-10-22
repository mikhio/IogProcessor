#include "spu_binary_code.h"
#include "spu_return_codes.h"
#include "spu_cmd_ids.h"

#include "iog_assert.h"
#include "iog_stack.h"
#include "iog_memlib.h"
#include "cli_colors.h"

#include <string.h>
#include <stdlib.h>


SpuReturnCode spu_bin_load(const char *filename, SpuBin_t *binCode) {
  IOG_ASSERT(filename);
  IOG_ASSERT(binCode);

  FILE *binfile = fopen(filename, "rb");
  
  if (!binfile) {
    fprintf(stderr, RED("ERROR: Can't open file %s\n"), filename);
    return ERR_CANT_OPEN_FILE;
  }


  fread(&binCode->size, sizeof(size_t), 1, binfile);

  binCode->code = (int *)iog_recalloc(
      binCode->code,
      0,
      binCode->size,
      sizeof(int)
  );
  
  fread(binCode->code, sizeof(int), binCode->size, binfile);


  fclose(binfile);

  return SPU_OK;
}

SpuReturnCode spu_bin_read_cmd(SpuBin_t *binCode, SpuCmdId_t *curCmd, int *value) {
  IOG_ASSERT(binCode);
  IOG_ASSERT(binCode->code);
  IOG_ASSERT(curCmd);
  IOG_ASSERT(value);

  if (binCode->ip >= binCode->size)
    return SPU_QUIT;

  *curCmd = (SpuCmdId_t) binCode->code[binCode->ip];
  binCode->ip++;

  if ((*curCmd == SPU_PUSH_ID) || (*curCmd == SPU_POP_ID) || (*curCmd == SPU_PUSHR_ID)) {
    *value = binCode->code[binCode->ip];
      binCode->ip++;
  } else {
    *value = 0;
  }


  return SPU_OK;
}


SpuReturnCode spu_bin_run (SpuBin_t *binCode) {
  IOG_ASSERT(binCode);

  iog_stack_init(&binCode->stack);

  binCode->ip = 0;
  SpuCmdId_t curCmd = SPU_NONE_ID;
  int cmdValue = 0;

  SpuReturnCode isRunning = SPU_OK;

  while (isRunning == SPU_OK) {
    isRunning = spu_bin_read_cmd(binCode, &curCmd, &cmdValue);

    if (isRunning != SPU_OK)
      break;

    switch (curCmd) {
      case SPU_PUSH_ID: {
        iog_stack_push(&binCode->stack, cmdValue);

        break;
      }
      case SPU_PUSHR_ID: {
        iog_stack_push(&binCode->stack, binCode->regs[cmdValue]);

        break;
      }
      case SPU_POP_ID: {
        iog_stack_pop(&binCode->stack, binCode->regs + cmdValue);

        break;
      }
      case SPU_ADD_ID: {
        int firstValue = 0, secondValue = 0;

        iog_stack_pop(&binCode->stack, &firstValue);
        iog_stack_pop(&binCode->stack, &secondValue);

        iog_stack_push(&binCode->stack, firstValue + secondValue);
        break;
      }
      case SPU_MUL_ID: {
        int firstValue = 0, secondValue = 0;

        iog_stack_pop(&binCode->stack, &firstValue);
        iog_stack_pop(&binCode->stack, &secondValue);

        iog_stack_push(&binCode->stack, firstValue * secondValue);
        break;
      }
      case SPU_OUT_ID: {
        int value = 0;

        iog_stack_pop(&binCode->stack, &value);

        printf("-");
        printf("%d", value);
        printf("-");
        break;
      }
      case SPU_HLT_ID: {
        isRunning = SPU_QUIT;
        break;
      } case SPU_NONE_ID: {
        fprintf(stderr, RED("ERROR: NONE command %d\n"), curCmd);
        break;
      }
      default: {
        fprintf(stderr, RED("ERROR: Unknown command %d\n"), curCmd);
        break;
      }
    }
  }

  iog_stack_destroy(&binCode->stack);

  return SPU_OK;
}


SpuReturnCode spu_bin_free (SpuBin_t *binCode) {
  IOG_ASSERT(binCode);

  free(binCode->code);
  binCode->code = NULL;
  binCode->size = 0;

  return SPU_OK;
}


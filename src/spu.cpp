#include "spu.h"
#include "spu_code.h"

#include "spu_return_codes.h"
#include "spu_cmd_ids.h"

#include "iog_assert.h"
#include "iog_stack.h"
#include "iog_memlib.h"
#include "cli_colors.h"

#include <string.h>
#include <stdlib.h>


SpuReturnCode spu_run (Spu_t *proc) {
  IOG_ASSERT(proc);

  iog_stack_init(&proc->stack);

  proc->ip = 0;
  cmd_code_t curCmd = GET_CMD_CODE(SPU_NONE_ARG_TYPE, SPU_NONE_ID);
  int cmdValue = 0;

  SpuReturnCode isRunning = SPU_OK;

  while (isRunning == SPU_OK) {
    isRunning = spu_read_cmd(proc, &curCmd, &cmdValue);

    fprintf(stderr, BLUE("[EXECUTING] ") " cmd: %2d = (ARG) %2d + (CMD_ID) %2d, value: %d\n", 
        curCmd,
        GET_ARG_TYPE(curCmd),
        GET_CMD_ID(curCmd),
        cmdValue
    );

    if (isRunning != SPU_OK)
      break;

    switch (curCmd) {
      case GET_CMD_CODE(SPU_DATA_TYPE, SPU_PUSH_ID): {
        iog_stack_push(&proc->stack, cmdValue);

        break;
      }
      case GET_CMD_CODE(SPU_REG_TYPE, SPU_PUSH_ID): {
        iog_stack_push(&proc->stack, proc->regs[cmdValue]);

        break;
      }
      case GET_CMD_CODE(SPU_REG_TYPE, SPU_POP_ID): {
        iog_stack_pop(&proc->stack, proc->regs + cmdValue);

        break;
      }
      case GET_CMD_CODE(SPU_NONE_ARG_TYPE, SPU_ADD_ID): {
        int firstValue = 0, secondValue = 0;

        iog_stack_pop(&proc->stack, &firstValue);
        iog_stack_pop(&proc->stack, &secondValue);

        iog_stack_push(&proc->stack, firstValue + secondValue);
        break;
      }
      case GET_CMD_CODE(SPU_NONE_ARG_TYPE, SPU_MUL_ID): {
        int firstValue = 0, secondValue = 0;

        iog_stack_pop(&proc->stack, &firstValue);
        iog_stack_pop(&proc->stack, &secondValue);

        iog_stack_push(&proc->stack, firstValue * secondValue);
        break;
      }
      case GET_CMD_CODE(SPU_NONE_ARG_TYPE, SPU_OUT_ID): {
        int value = 0;

        iog_stack_peek(&proc->stack, &value);

        printf("-");
        printf("%d", value);
        printf("-");
        printf("\n");
        break;
      }
      case GET_CMD_CODE(SPU_NONE_ARG_TYPE, SPU_HLT_ID): {
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

  iog_stack_destroy(&proc->stack);

  return SPU_OK;
}

SpuReturnCode spu_read_cmd(Spu_t *proc, cmd_code_t *curCmd, int *value) {
  IOG_ASSERT(proc);
  IOG_ASSERT(proc->code.buffer);
  IOG_ASSERT(curCmd);
  IOG_ASSERT(value);

  if (proc->ip >= proc->code.bufSize)
    return SPU_QUIT;

  *curCmd = (cmd_code_t) proc->code.buffer[proc->ip];
  proc->ip++;

  if (GET_ARG_TYPE(*curCmd) != SPU_NONE_ARG_TYPE) {
    *value = proc->code.buffer[proc->ip];
    proc->ip++;
  } else {
    *value = 0;
  }

  return SPU_OK;
}

SpuReturnCode spu_free (Spu_t *proc) {
  IOG_ASSERT(proc);

  spu_code_free(&proc->code);

  return SPU_OK;
}


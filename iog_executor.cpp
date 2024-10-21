#include "iog_assert.h"
#include "iog_stack.h"
#include "cli_colors.h"
#include "iog_memlib.h"
#include "string.h"
#include "stdlib.h"

const char *DEFAULT_TEST_FILE = "tests/my.bin";

enum SpuReturnCode {
  SPU_OK               = 0,
  SPU_QUIT             = 1,
  ERR_MANY_ARGUMENTS   = 2,
  ERR_CANT_OPEN_FILE   = 3,

  NR_SPU_RETURN_CODE   = 4,
};

enum SpuCmdId {
  SPU_NONE_ID = 0,
  SPU_PUSH_ID = 1,
  SPU_ADD_ID  = 2,
  SPU_OUT_ID  = 3,
  SPU_HLT_ID  = 4,
  SPU_MUL_ID  = 5,
};

struct SpuBin_t {
  IogStack_t stack;
  int *code;
  size_t ip;
  size_t size;
};

SpuReturnCode spu_handle_args (int argc, const char *argv[], const char **filename) {
  IOG_ASSERT(argc > 0);
  IOG_ASSERT(argv);

  if (argc > 2) {
    fprintf(stderr, RED("ERROR: Too many arguments\n"));
    return ERR_MANY_ARGUMENTS;
  }

  if (argc <= 1) {
    *filename = DEFAULT_TEST_FILE;
  } else {
    *filename = argv[argc-1];
  }

  return SPU_OK;
}

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

SpuReturnCode spu_bin_read_cmd(SpuBin_t *binCode, SpuCmdId *curCmd, int *value) {
  IOG_ASSERT(binCode);
  IOG_ASSERT(binCode->code);
  IOG_ASSERT(curCmd);
  IOG_ASSERT(value);

  if (binCode->ip >= binCode->size)
    return SPU_QUIT;

  *curCmd = (SpuCmdId) binCode->code[binCode->ip];
  binCode->ip++;

  if (*curCmd == SPU_PUSH_ID) {
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
  SpuCmdId curCmd = SPU_NONE_ID;
  int cmdValue = 0;

  SpuReturnCode isRunning = SPU_OK;

  while (isRunning == SPU_OK) {
    isRunning = spu_bin_read_cmd(binCode, &curCmd, &cmdValue);

    if (isRunning != SPU_OK)
      break;

    switch (curCmd) {
      case SPU_PUSH_ID: {
        iog_stack_push(&binCode->stack, cmdValue);

        cmdValue = 0;
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


int main (int argc, const char *argv[]) {
  const char *filename = NULL;
  spu_handle_args(argc, argv, &filename);

  SpuBin_t binCode = {};

  spu_bin_load(filename, &binCode);

  spu_bin_run(&binCode);

  spu_bin_free(&binCode);

  return 0;
}

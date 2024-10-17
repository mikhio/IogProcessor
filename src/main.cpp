#include "iog_assert.h"
#include "iog_stack.h"
#include "cli_colors.h"
#include "iog_memlib.h"
#include "string.h"
#include "stdlib.h"

const char *DEFAULT_TEST_FILE = "tests/code_example.iog";
const int   CAPACITY_CHUNK    = 100;

enum SpuReturnCode {
  SPU_OK               = 0,
  ERR_MANY_ARGUMENTS   = 1,
  ERR_CANT_OPEN_FILE   = 2,
  ERR_CANT_CODE_APPEND = 3,

  NR_SPU_RETURN_CODE   = 4,
};

enum SpuCmdId {
  SPU_PUSH_ID = 1,
  SPU_ADD_ID  = 2,
  SPU_OUT_ID  = 3,
  SPU_HLT_ID  = 4,
  SPU_MUL_ID  = 5,
};

struct SpuCode_t {
  int *buffer;

  size_t bufSize;
  size_t bufCapacity;
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

SpuReturnCode spu_code_allocate (SpuCode_t *code, size_t newCapacity) {
  IOG_ASSERT(code);
  IOG_ASSERT(newCapacity >= code->bufSize);

  int *tmp_ptr = (int *) iog_recalloc(
      code->buffer,
      code->bufCapacity,
      newCapacity,
      sizeof(int)
  );

  if (tmp_ptr == NULL) {
    fprintf(stderr, RED("ERROR: Can't recallocate code buffer!"));
    return ERR_CANT_CODE_APPEND;
  }
  
  code->buffer = tmp_ptr;
  code->bufCapacity = newCapacity;

  return SPU_OK;
}

SpuReturnCode spu_code_append(SpuCode_t *code, int value) {
  IOG_ASSERT(code);

  if (code->bufSize == code->bufCapacity) {
    spu_code_allocate(code, code->bufCapacity + CAPACITY_CHUNK);
  }

  code->buffer[code->bufSize] = value;
  code->bufSize++;

  return SPU_OK;
}


SpuReturnCode spu_compile_code (const char *filename, SpuCode_t *code) {
  IOG_ASSERT(filename);
  IOG_ASSERT(code);

  FILE *codefile = fopen(filename, "rb");

  if (!codefile) {
    fprintf(stderr, RED("ERROR: Can't open file with code!\n"));
    return ERR_CANT_OPEN_FILE;
  }

  char cmd[100] = {};

  while (fscanf(codefile, "%s", cmd) != EOF) {
    if (strcmp(cmd, "push") == 0) {
      int num = 0;
      if (fscanf(codefile, "%d", &num) == 0) {
        fprintf(stderr, RED("SyntaxError: not number after push\n"));
        break;
      }

      spu_code_append(code, (int) SPU_PUSH_ID);
      spu_code_append(code, num);
    } else if (strcmp(cmd, "add") == 0) {
      spu_code_append(code, (int) SPU_ADD_ID);
    } else if (strcmp(cmd, "out") == 0) {
      spu_code_append(code, (int) SPU_OUT_ID);
    } else {
      fprintf(stderr, RED("SyntaxError: Unknonw command %s\n"), cmd);
      break;
    }
  }

  fclose(codefile);

  spu_code_append(code, -1);
  spu_code_allocate(code, code->bufSize);

  return SPU_OK;
}

SpuReturnCode spu_code_free (SpuCode_t *code) {
  IOG_ASSERT(code);

  free(code->buffer);
  code->buffer = NULL;
  code->bufCapacity = 0;
  code->bufSize = 0;

  return SPU_OK;
}

SpuReturnCode spu_run (SpuCode_t *code) {
  return SPU_OK;
}

int main (int argc, const char *argv[]) {
  const char *filename = NULL;
  spu_handle_args(argc, argv, &filename);

  SpuCode_t code = {};

  spu_compile_code(filename, &code);

  for (size_t i = 0; i < code.bufCapacity; i++) {
    fprintf(stderr, BLACK("code.buffer[%lu] = %d\n"), i, code.buffer[i]);
  }

  spu_code_save(code, "my.out");

  spu_code_free(&code);

  return 0;
}

#include "spu_code.h"
#include "spu_return_codes.h"
#include "spu_cmd_ids.h"
#include "cli_colors.h"

#include "iog_assert.h"
#include "iog_memlib.h"

#include <string.h>
#include <stdlib.h>


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

SpuReturnCode spu_code_save (const SpuCode_t *code, const char *filename) {
  IOG_ASSERT(code);
  IOG_ASSERT(filename);

  FILE *binfile = fopen(filename, "wb");

  if (!binfile) {
    fprintf(stderr, RED("ERROR: Can't open binary file %s\n"), filename);
    return ERR_CANT_SAVE_CODE;
  }

  fwrite(&code->bufSize, sizeof(size_t), 1, binfile);
  fwrite(code->buffer,  sizeof(int), code->bufSize, binfile);

  fclose(binfile);

  return SPU_OK;
}

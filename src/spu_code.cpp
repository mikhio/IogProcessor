#include "spu_code.h"
#include "spu_text_reader.h"
#include "spu_return_codes.h"
#include "spu_cmd_ids.h"
#include "spu_commands.h"

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
    fprintf(stderr, RED("ERROR: Can't recallocate code buffer!\n"));
    return ERR_CANT_APPEND;
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


SpuReturnCode spu_code_compile (const char *filename, SpuCode_t *code) {
  IOG_ASSERT(filename);
  IOG_ASSERT(code);

  SpuText_t text = {};

  spu_text_read(filename, &text);

  for (int line = 0; line < text.linesSize; line++) {
    fprintf(stderr, BLUE("[PROCESSING] ") "'%s'\n", text.lines[line].line);
    
    char cmd[100] = {};
    int num = 0;

    if (sscanf(text.lines[line].line, "%s", cmd) == 0) {
      fprintf(stderr, RED("ReadingError: beaten line \n"));
      continue;
    }

    for (int i = 0; i < SPU_CMDS_AMOUNT; i++) {
      if (strcmp(cmd, SPU_CMDS[i].name) == 0) {
        cmd_code_t cmdCode = GET_CMD_CODE(SPU_NONE_ARG_TYPE, SPU_CMDS[i].id);

        if (SPU_CMDS[i].argsNum == 1) {
          cmdCode |= GET_ARG_TYPE( SPU_CMDS[i].handle_args(&text.lines[line], &num) );
        }

        spu_code_append(code, (int) cmdCode);

        if (GET_ARG_TYPE(cmdCode) != SPU_NONE_ARG_TYPE) {
          spu_code_append(code, num);
        }

        break;
      } else if (strcmp(cmd, "") == 0) {
        continue;
      }

      if (i == SPU_CMDS_AMOUNT - 1)
        fprintf(stderr, RED("SyntaxError: command not found\n"));
    }
  }

  spu_text_free(&text);

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

SpuReturnCode spu_code_load (const char *filename, SpuCode_t *code) {
  IOG_ASSERT(filename);
  IOG_ASSERT(code);

  FILE *binfile = fopen(filename, "rb");
  
  if (!binfile) {
    fprintf(stderr, RED("ERROR: Can't open file %s\n"), filename);
    return ERR_CANT_OPEN_FILE;
  }


  fread(&code->bufSize, sizeof(size_t), 1, binfile);

  code->buffer = (int *)iog_recalloc(
      code->buffer,
      0,
      code->bufSize,
      sizeof(int)
  );
  
  fread(code->buffer, sizeof(int), code->bufSize, binfile);


  fclose(binfile);

  return SPU_OK;
}


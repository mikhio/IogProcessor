#include "spu_code.h"
#include "spu_text_reader.h"
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
    fprintf(stderr, RED("ERROR: Can't recallocate code buffer!\n"));
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
    } else if (strcmp(cmd, "push") == 0) {
      SpuReturnCode argType = handle_push_args(&text.lines[line], &num);

      if (argType == SPU_REG_TYPE) {
        spu_code_append(code, (int) SPU_PUSHR_ID);
      } else if (argType == SPU_NUM_TYPE) {
        spu_code_append(code, (int) SPU_PUSH_ID);
      }

      spu_code_append(code, num);
    } else if (strcmp(cmd, "pop") == 0) {
      handle_pop_args(&text.lines[line], &num);

      spu_code_append(code, (int) SPU_POP_ID);
      spu_code_append(code, num);
    } else if (strcmp(cmd, "add") == 0) {
      spu_code_append(code, (int) SPU_ADD_ID);
    } else if (strcmp(cmd, "mul") == 0) {
      spu_code_append(code, (int) SPU_MUL_ID);
    } else if (strcmp(cmd, "out") == 0) {
      spu_code_append(code, (int) SPU_OUT_ID);
    } else if (strcmp(cmd, "hlt") == 0) {
      spu_code_append(code, (int) SPU_HLT_ID);
    } else if (strcmp(cmd, "jmp") == 0) {
      spu_code_append(code, (int) SPU_JMP_ID);
    } else if (strcmp(cmd, "ja") == 0) {
      spu_code_append(code, (int) SPU_JA_ID);
    } else if (strcmp(cmd, "jae") == 0) {
      spu_code_append(code, (int) SPU_JAE_ID);
    } else if (strcmp(cmd, "jb") == 0) {
      spu_code_append(code, (int) SPU_JB_ID);
    } else if (strcmp(cmd, "jbe") == 0) {
      spu_code_append(code, (int) SPU_JBE_ID);
    } else if (strcmp(cmd, "je") == 0) {
      spu_code_append(code, (int) SPU_JE_ID);
    } else if (strcmp(cmd, "jne") == 0) {
      spu_code_append(code, (int) SPU_JNE_ID);
    } else if (strcmp(cmd, "") == 0) {
      continue;
    } else {
      fprintf(stderr, RED("SyntaxError: Unknonw command '%s'\n"), cmd);
      break;
    }
    
  }

  spu_text_free(&text);

  spu_code_allocate(code, code->bufSize);

  return SPU_OK;
}

static SpuReturnCode handle_push_args (StrLine *line, int *num) {
  IOG_ASSERT(line);
  IOG_ASSERT(num);

  char reg_name[10];
  char cmd[10];

  if (sscanf(line->line, "%s %d", cmd, num) == 2) {
    return SPU_NUM_TYPE;
  } else if (sscanf(line->line, "%s %s", cmd, reg_name) == 2) {
      if (convert_reg_name_to_num(reg_name, num) == SPU_OK)
        return SPU_REG_TYPE;
  }
  
  fprintf(stderr, RED("SyntaxError: neither number nor register name after push\n"));

  return ERR_CANT_HANDLE_ARG;
}

static SpuReturnCode handle_pop_args (StrLine *line, int *num) {
  IOG_ASSERT(line);
  IOG_ASSERT(num);

  char reg_name[10];
  char cmd[10];

  if (sscanf(line->line, "%s %s", cmd, reg_name) == 1) {
    fprintf(stderr, RED("SyntaxError: Can't find register after pop\n"));
    return ERR_CANT_HANDLE_ARG;
  }

  if (convert_reg_name_to_num(reg_name, num) != SPU_OK) {
    fprintf(stderr, RED("SyntaxError: Can't read register after pop\n"));
    return ERR_CANT_HANDLE_ARG;
  }

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

static SpuReturnCode convert_reg_name_to_num (char *reg_name, int *num) {
  IOG_ASSERT(reg_name);
  IOG_ASSERT(num);

  if ( !((reg_name[0] >= 'a') && (reg_name[0] <= 'z') && (reg_name[1] == 'x')) ) {
    fprintf(stderr, RED("Error: Unknown register name %s\n"), reg_name);
    return ERR_NOT_REGISTER;
  }


  *num = (int) (reg_name[0] - 'a');

  return SPU_OK;
}

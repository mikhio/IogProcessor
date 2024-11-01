#include "spu_code.h"
#include "spu_text_reader.h"
#include "spu_return_codes.h"
#include "spu_cmd_ids.h"
#include "spu_commands.h"
#include "spu_label.h"
#include "spu_constants.h"

#include "cli_colors.h"
#include "iog_assert.h"
#include "iog_memlib.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>


SpuReturnCode spu_code_compile (const char *filename, SpuCode_t *code) {
  IOG_ASSERT(filename);
  IOG_ASSERT(code);

  SpuText_t text = {};

  spu_text_read(filename, &text);

  SpuLabels_t labels = {};
  spu_labels_init(&labels);

  if (detectAllLabels(&text, &labels) != SPU_OK) {
    fprintf(stderr, RED("CompileError: can't run label's detector"));
  }

  for (size_t line = 0; line < text.linesSize; line++) {
    fprintf(stderr, BLUE("[PROCESSING] ") "'%s'\n", text.lines[line].line);
    
    char cmd[MAX_WORD_NAME] = {};
    int num = 0;

    if (sscanf(text.lines[line].line, "%s", cmd) == 0) {
      fprintf(stderr, RED("ReadingError: beaten line \n"));
      continue;
    }

    if (checkLabelName(cmd) == SPU_OK) {
      fprintf(stderr, BLACK("  [LABEL FOUND]\n"));
      continue;
    }
    

    for (size_t i = 0; i < SPU_CMDS_AMOUNT; i++) {
      if (strcmp(cmd, SPU_CMDS[i].name) == 0) {
        cmd_code_t cmdCode = GET_CMD_CODE(SPU_NONE_ARG_TYPE, SPU_CMDS[i].id);

        if (SPU_CMDS[i].argsNum == 1) {
          if (SPU_CMDS[i].isJump == 1)
            cmdCode |= GET_ARG_TYPE( SPU_CMDS[i].handle_args(&text.lines[line], &num, &labels) );
          else
            cmdCode |= GET_ARG_TYPE( SPU_CMDS[i].handle_args(&text.lines[line], &num, NULL) );
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

  spu_labels_dump(&labels);

  spu_text_free(&text);
  spu_labels_free(&labels);

  spu_code_allocate(code, code->bufSize); // Free useless memory

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

static SpuReturnCode detectAllLabels (const SpuText_t *text, SpuLabels_t *labels) {
  IOG_ASSERT(text);
  IOG_ASSERT(labels);

  char word[MAX_WORD_NAME] = {};
  int cmdCount = 0;

  for (size_t line = 0; line < text->linesSize; line++) {
    if (sscanf(text->lines[line].line, "%s", word) > 0) {
      int cmdArgsNum = 0;

      if (checkLabelName(word) == SPU_OK) {
        spu_labels_append(labels, word, cmdCount);
      } else if (checkCmdName(word, &cmdArgsNum) == SPU_OK) {
        cmdCount += 1 + cmdArgsNum;
      }
    }
  }

  return SPU_OK;
}

static SpuReturnCode checkLabelName (char *word) {
  IOG_ASSERT(word);

  if (strlen(word) > MAX_LABEL_NAME)
    return ERR_INCORRECT;

  char *labelEnd = strstr(word, ":");
  if ((isalpha(word[0]) != 0) && (labelEnd != NULL)) {
    return SPU_OK;
  }

  return ERR_INCORRECT;
}

static SpuReturnCode checkCmdName (char *word, int *cmdArgsNum) {
  IOG_ASSERT(word);

  for (size_t i = 0; i < SPU_CMDS_AMOUNT; i++) {
    if (strcmp(word, SPU_CMDS[i].name) == 0) {
      *cmdArgsNum = SPU_CMDS[i].argsNum;
      return SPU_OK;
    }
  }

  return ERR_INCORRECT;
}

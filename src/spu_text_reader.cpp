#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "spu_text_reader.h"
#include "spu_return_codes.h"

#include "iog_assert.h"
#include "iog_memlib.h"
#include "cli_colors.h"


SpuReturnCode spu_text_read (const char *filename, SpuText_t *text) {
  IOG_ASSERT(filename);
  IOG_ASSERT(text);

  FILE *textFile = fopen(filename, "rb");

  if (!textFile) {
    fprintf(stderr, RED("ERROR: ") "Can't open file: " BLACK("%s") "\n", filename);
    return ERR_CANT_READ_FILE;
  }

  if (fill_buffer(textFile, text) != SPU_OK) {
    fprintf(stderr, RED("ERROR: ") "Can't read text from file: " BLACK("%s") "\n", filename);

    fclose(textFile);
    return ERR_CANT_FILL_BUFFER;
  }
  
  if (split_to_lines(text) != SPU_OK) {
    fprintf(stderr, MAGENTA("Warrning: ") "Can't split text to lines" "\n");
  }

  fclose(textFile);

  return SPU_OK;
}

static SpuReturnCode fill_buffer (FILE *textFile, SpuText_t *text) {
  IOG_ASSERT(textFile);
  IOG_ASSERT(text);

  size_t fileSize = 0;

  if (read_file_size(textFile, &fileSize) != SPU_OK) {
    return ERR_CANT_READ_FILE;
  }

  if (allocate_buffer(text, fileSize + 1) != SPU_OK) {
    fprintf(stderr, RED("ERROR: ") "Can't allocate buffer for text" "\n");

    return ERR_CANT_ALLOCATE;
  }

  if (fread(text->buffer, sizeof(char), fileSize, textFile) < fileSize)
    return ERR_CANT_READ_FILE;

  text->bufferSize = fileSize;
  text->buffer[text->bufferSize] = 0;

  return SPU_OK;
}

static SpuReturnCode read_file_size (FILE *textFile, size_t *fileSize) {
  IOG_ASSERT(textFile); 
  IOG_ASSERT(fileSize); 
  
  int textFd = fileno(textFile);
  struct stat statBuf = {};

  if (fstat(textFd, &statBuf) != 0)
    return ERR_CANT_READ_FILE;

  printf(BLACK("------ READING FILE STAT ------- \n"));
  printf(BLUE("[INFO]") BLACK(" ST_SIZE:    ")  "%lld" "\n", statBuf.st_size); 
  printf(BLUE("[INFO]") BLACK(" ST_BLKSIZE: ")  "%d"   "\n", statBuf.st_blksize); 
  printf(BLUE("[INFO]") BLACK(" ST_BLOCKS:  ")  "%lld" "\n", statBuf.st_blocks); 
  printf("\n");

  *fileSize = statBuf.st_size;

  return SPU_OK; 
}


static SpuReturnCode split_to_lines (SpuText_t *text) {
  IOG_ASSERT(text);

  size_t linesNum = 0;
  
  if (count_lines(text, &linesNum) != SPU_OK)
    return ERR_CANT_SPLIT;

  if (allocate_lines(text, linesNum+1) != SPU_OK)
    return ERR_CANT_ALLOCATE;
  
  printf(BLACK("------ SPLITING ------- \n"));
  printf(BLUE("[INFO] ") BLACK("linesNum      ") "= %lu\n", linesNum);
  printf(BLUE("[INFO] ") BLACK("linesSize     ") "= %lu\n", text->linesSize);
  printf(BLUE("[INFO] ") BLACK("linesCapacity ") "= %lu\n", text->linesCapacity);
  printf("\n");
  
  text->lines[0].line = text->buffer;

  size_t lineCount = 1; 
  for (size_t i = 0; i < text->bufferSize; i++) {
    if (text->buffer[i] == '\n') { 
      if (lineCount >= linesNum)
        return ERR_CANT_SPLIT;

      text->lines[lineCount].line = text->buffer + i + 1;
      text->lines[lineCount-1].length = text->lines[lineCount].line - text->lines[lineCount-1].line - 1;
      text->buffer[i] = '\0';

      printf(GREEN("[READ] ") BLACK("%p %p") " '%s' size: %lu\n", 
          &text->lines[lineCount-1].line,
          text->lines[lineCount-1].line,
          text->lines[lineCount-1].line, 
          text->lines[lineCount-1].length
      );
      lineCount++;
    }
  }
  
  text->linesSize = lineCount;

  printf("\n");
  printf(BLUE("[INFO] ") BLACK("lineCount") " = %lu -> " BLACK("linesSize") " = %lu\n", lineCount, text->linesSize);
  printf("\n");

  return SPU_OK;
}

static SpuReturnCode count_lines (SpuText_t *text, size_t *linesNum) {
  IOG_ASSERT(text);
  IOG_ASSERT(linesNum);

  if (!text->buffer)
    return ERR_NULL_BUFFER;

  size_t lineCount = 0; 
  for (int i = 0; i < text->bufferSize+1; i++) {
    if ((text->buffer[i] == '\n') || (text->buffer[i] == '\0'))
      lineCount++;
  }

  *linesNum = lineCount;

  return SPU_OK;
}

static SpuReturnCode allocate_lines (SpuText_t *text, size_t newCapacity) {
  IOG_ASSERT(text);
  
  StrLine *tmp = (StrLine *) iog_recalloc(
      text->lines,
      text->linesCapacity,
      newCapacity,
      sizeof(StrLine)
  );

  if (tmp == NULL)
    return ERR_CANT_ALLOCATE;

  text->lines = tmp;
  text->linesCapacity = newCapacity;

  return SPU_OK;
}


static SpuReturnCode allocate_buffer (SpuText_t *text, size_t newCapacity) {
  IOG_ASSERT(text);

  char *tmp = (char *) iog_recalloc(
      text->buffer,
      text->bufferCapacity,
      newCapacity,
      sizeof(char)
  );

  if (tmp == NULL)
    return ERR_CANT_ALLOCATE;

  text->buffer = tmp;
  text->bufferCapacity = newCapacity;

  return SPU_OK;
}

SpuReturnCode spu_text_free (SpuText_t *text) {
  IOG_ASSERT(text);

  free(text->lines);
  text->lines = NULL;
  text->linesSize = 0;
  text->linesCapacity = 0;

  free(text->buffer);
  text->buffer = NULL;
  text->bufferSize = 0;
  text->bufferCapacity = 0;

  return SPU_OK;
}

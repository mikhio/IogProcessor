#ifndef SPU_CODE_READER_H
#define SPU_CODE_READER_H

#include <stdio.h>
#include <stdlib.h>

#include "spu_return_codes.h"

struct StrLine {
  char  *line;
  size_t length;
};

struct SpuText_t {
  char   *buffer;
  size_t  bufferCapacity;
  size_t  bufferSize;

  StrLine *lines;
  size_t  linesCapacity;
  size_t  linesSize;
};

SpuReturnCode spu_text_read  (const char *filename, SpuText_t *text);
SpuReturnCode spu_text_free  (SpuText_t *text);

static SpuReturnCode read_file_size (FILE *textFile, size_t *fileSize);
static SpuReturnCode fill_buffer   (FILE *textFile, SpuText_t *text);

static SpuReturnCode split_to_lines (SpuText_t *text);
static SpuReturnCode count_lines    (SpuText_t *text, size_t *linesNum);

static SpuReturnCode allocate_lines  (SpuText_t *text, size_t newCapacity);
static SpuReturnCode allocate_buffer (SpuText_t *text, size_t newCapacity);

#endif // SPU_CODE_READER_H

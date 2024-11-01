#ifndef SPU_LABEL_H
#define SPU_LABEL_H

#include "spu_return_codes.h"

#include <stdlib.h>

const size_t SPU_LABELS_CHUNK = 10;
const size_t MAX_LABEL_NAME   = 50;

struct SpuLabel_t {
  char name[MAX_LABEL_NAME];
  int addr;
};

struct SpuLabels_t {
  SpuLabel_t *arr;
  size_t size;
  size_t capacity;
};

SpuReturnCode spu_labels_init   (SpuLabels_t *labels);
SpuReturnCode spu_labels_free   (SpuLabels_t *labels);
SpuReturnCode spu_labels_dump   (const SpuLabels_t *labels);

SpuReturnCode spu_labels_append (SpuLabels_t *labels, const char *name, int addr);

static SpuReturnCode allocateMore (SpuLabels_t *labels, size_t chunkSize);

#endif // SPU_LABEL_H

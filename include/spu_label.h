#ifndef SPU_LABEL_H
#define SPU_LABEL_H

#include "spu_return_codes.h"

#include <stdlib.h>

const size_t SPU_LABELS_CHUNK = 10;

struct SpuLabel_t {
  char *name;
  int addr;
};

struct SpuLabels_t {
  SpuLabel_t *arr;
  size_t size;
  size_t capacity;
};

SpuReturnCode spu_labels_init   (SpuLabels_t *labels, size_t initSize);
SpuReturnCode spu_labels_append (SpuLabels_t *labels, const SpuLabel_t *label);
SpuReturnCode spu_labels_free   (SpuLabels_t *labels);

static SpuReturnCode allocateMore (SpuLabels_t *labels, size_t chunkSize);

#endif // SPU_LABEL_H

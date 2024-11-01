#include "spu_label.h"
#include "spu_return_codes.h"

#include "cli_colors.h"
#include "iog_memlib.h"
#include "iog_assert.h"

#include <stdio.h>
#include <stdlib.h>


SpuReturnCode spu_labels_init (SpuLabels_t *labels, size_t initSize) {
  IOG_ASSERT(labels);

  labels->arr      = NULL;
  labels->size     = 0;
  labels->capacity = 0;

  if (allocateMore(labels, SPU_LABELS_CHUNK) != SPU_OK) {
    fprintf(stderr, RED("InitError: can't allocate memory for label's array\n"));
    return ERR_CANT_INIT;
  }

  return SPU_OK;
}

SpuReturnCode spu_labels_append (SpuLabels_t *labels, const SpuLabel_t *label) {
  IOG_ASSERT(labels);

  if (labels->size >= labels->capacity) {
    if (allocateMore(labels, SPU_LABELS_CHUNK) != SPU_OK) {
      fprintf(stderr, RED("AppendError: can't ensize label's array to append\n"));
      return ERR_CANT_APPEND;
    }
  }

  labels->arr[labels->size].name = label->name;
  labels->arr[labels->size].addr = label->addr;
  labels->size++;

  return SPU_OK;
}

SpuReturnCode spu_labels_free (SpuLabels_t *labels) {
  IOG_ASSERT(labels);

  free(labels->arr);
  labels->size = 0;
  labels->capacity = 0;

  return SPU_OK;
}

static SpuReturnCode allocateMore (SpuLabels_t *labels, size_t chunkSize) {
  IOG_ASSERT(labels);

  SpuLabel_t *tmp_ptr = (SpuLabel_t *) iog_recalloc(
      labels->arr,
      labels->capacity,
      labels->capacity + chunkSize,
      sizeof (SpuLabel_t)
  );

  if (tmp_ptr == NULL) {
    return ERR_CANT_ALLOCATE;
  }

  labels->arr = tmp_ptr;
  labels->capacity += chunkSize;

  return SPU_OK;
}

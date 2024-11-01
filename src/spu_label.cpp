#include "spu_label.h"
#include "spu_return_codes.h"
#include "spu_constants.h"

#include "cli_colors.h"
#include "iog_memlib.h"
#include "iog_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


SpuReturnCode spu_labels_init (SpuLabels_t *labels) {
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

SpuReturnCode spu_labels_append (SpuLabels_t *labels, const char *name, int addr) {
  IOG_ASSERT(labels);

  if (labels->size >= labels->capacity) {
    if (allocateMore(labels, SPU_LABELS_CHUNK) != SPU_OK) {
      fprintf(stderr, RED("AppendError: can't ensize label's array to append\n"));
      return ERR_CANT_APPEND;
    }
  }

  if (strlen(name) > MAX_LABEL_NAME) {
    fprintf(stderr, RED("AppendError: label name more than MAX_LABEL_NAME\n"));
    return ERR_CANT_APPEND;
  }

  strcpy(labels->arr[labels->size].name, name);
  labels->arr[labels->size].addr = addr;
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

SpuReturnCode spu_labels_dump (const SpuLabels_t *labels) {
  IOG_ASSERT(labels);

  if (labels->arr == NULL) {
    fprintf(stderr, BLACK("Lables: (NULL)\n"));
    return SPU_OK;
  } else {
    fprintf(stderr, BLACK("Lables: (%p)\n"), labels->arr);
  }

  for (size_t i = 0; i < labels->size; i++) {
    fprintf(stderr, BLACK("  label[%lu]: name = '%s', addr = %d\n"), i, labels->arr[i].name, labels->arr[i].addr);
  }
  fprintf(stderr, "\n");

  return SPU_OK;
}

SpuLabel_t *spu_find_label (const SpuLabels_t *labels, const char *name) {
  IOG_ASSERT(labels);
  IOG_ASSERT(name);

  for (size_t i = 0; i < labels->size; i++) {
    if (strcmp(labels->arr[i].name, name) == 0) {
      return labels->arr + i;
    }
  }

  return NULL;
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


#include "iog_assert.h"
#include "cli_colors.h"

#include "spu.h"

const char *DEFAULT_TEST_FILE = "tests/my.bin";

SpuReturnCode spu_handle_args (int argc, const char *argv[], const char **filename) {
  IOG_ASSERT(argc > 0);
  IOG_ASSERT(argv);

  if (argc > 2) {
    fprintf(stderr, RED("ERROR: Too many arguments\n"));
    return ERR_MANY_ARGUMENTS;
  }

  if (argc <= 1) {
    *filename = DEFAULT_TEST_FILE;
  } else {
    *filename = argv[argc-1];
  }

  return SPU_OK;
}


int main (int argc, const char *argv[]) {
  const char *filename = NULL;
  spu_handle_args(argc, argv, &filename);

  Spu_t proc = {};

  spu_code_load(filename, &proc.code);

  spu_run(&proc);

  spu_free(&proc);

  return 0;
}

#include "iog_assert.h"
#include "cli_colors.h"
#include "spu_binary_code.h"
#include "spu_return_codes.h"

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

  SpuBin_t binCode = {};

  spu_bin_load(filename, &binCode);

  spu_bin_run(&binCode);

  spu_bin_free(&binCode);

  return 0;
}

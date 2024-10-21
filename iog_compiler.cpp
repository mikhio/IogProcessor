#include "iog_assert.h"
#include "cli_colors.h"
#include "iog_memlib.h"

#include "spu_return_codes.h"
#include "spu_code.h"

const char *DEFAULT_TEST_FILE = "tests/code_example.iog";

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

  SpuCode_t code = {};

  spu_compile_code(filename, &code);

  for (size_t i = 0; i < code.bufCapacity; i++) {
    fprintf(stderr, BLACK("code.buffer[%lu] = %d\n"), i, code.buffer[i]);
  }

  spu_code_save(&code, "tests/my.bin");

  spu_code_free(&code);

  return 0;
}

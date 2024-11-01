#include "spu_args_handlers.h"
#include "spu_text_reader.h"
#include "spu_cmd_ids.h"

#include "cli_colors.h"
#include "iog_assert.h"

#include <stdio.h>

cmd_code_t handle_push_args (StrLine *line, int *num) {
  IOG_ASSERT(line);
  IOG_ASSERT(num);

  char reg_name[10];
  char cmd[10];

  if (sscanf(line->line, "%s %d", cmd, num) == 2) {
    return SPU_DATA_TYPE;
  } else if (sscanf(line->line, "%s %s", cmd, reg_name) == 2) {
      if (convert_reg_name_to_num(reg_name, num) == SPU_OK)
        return SPU_REG_TYPE;
  }
  
  fprintf(stderr, RED("SyntaxError: neither number nor register name after push\n"));

  return SPU_NONE_ARG_TYPE;
}

cmd_code_t handle_pop_args (StrLine *line, int *num) {
  IOG_ASSERT(line);
  IOG_ASSERT(num);

  char reg_name[10];
  char cmd[10];

  if (sscanf(line->line, "%s %s", cmd, reg_name) == 1) {
    fprintf(stderr, RED("SyntaxError: Can't find register after pop\n"));
    return SPU_NONE_ARG_TYPE;
  }

  if (convert_reg_name_to_num(reg_name, num) != SPU_OK) {
    fprintf(stderr, RED("SyntaxError: Can't read register after pop\n"));
    return SPU_NONE_ARG_TYPE;
  }

  return SPU_REG_TYPE;
}

static SpuReturnCode convert_reg_name_to_num (char *reg_name, int *num) {
  IOG_ASSERT(reg_name);
  IOG_ASSERT(num);

  if ( !((reg_name[0] >= 'a') && (reg_name[0] <= 'z') && (reg_name[1] == 'x')) ) {
    fprintf(stderr, RED("Error: Unknown register name %s\n"), reg_name);
    return ERR_NOT_REGISTER;
  }

  *num = (int) (reg_name[0] - 'a');

  return SPU_OK;
}

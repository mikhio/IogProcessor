#include "spu_args_handlers.h"
#include "spu_text_reader.h"
#include "spu_cmd_ids.h"
#include "spu_constants.h"
#include "spu_label.h"

#include "cli_colors.h"
#include "iog_assert.h"

#include <stdio.h>
#include <string.h>

cmd_code_t handle_push_args (const StrLine *line, int *num, const SpuLabels_t *labels) {
  IOG_ASSERT(line);
  IOG_ASSERT(num);

  char arg[MAX_ARG_NAME];
  char cmd[MAX_CMD_NAME];

  if (sscanf(line->line, "%s %d", cmd, num) == 2) {
    return SPU_DATA_TYPE;
  } else if (sscanf(line->line, "%s %s", cmd, arg) == 2) {
    cmd_code_t ram_arg_type = SPU_NONE_ARG_TYPE;

    if (convert_reg_name_to_num(arg, num) == SPU_OK)
      return SPU_REG_TYPE;
    else if (convert_ram_addr_to_num(arg, num, &ram_arg_type) == SPU_OK)
      return SPU_ADDR_TYPE | ram_arg_type;
    else if (convert_char_to_num(arg, num) == SPU_OK)
      return SPU_DATA_TYPE;
  }
  
  fprintf(stderr, RED("SyntaxError: neither data nor register name after push\n"));

  return SPU_NONE_ARG_TYPE;
}

cmd_code_t handle_pop_args  (const StrLine *line, int *num, const SpuLabels_t *labels) {
  IOG_ASSERT(line);
  IOG_ASSERT(num);

  char arg[MAX_ARG_NAME];
  char cmd[MAX_WORD_NAME];

  if (sscanf(line->line, "%s %s", cmd, arg) == 1) {
    fprintf(stderr, RED("SyntaxError: Can't find register after pop\n"));
    return SPU_NONE_ARG_TYPE;
  }

  cmd_code_t ram_arg_type = SPU_NONE_ARG_TYPE;

  if (convert_reg_name_to_num(arg, num) == SPU_OK) {
    return SPU_REG_TYPE;
  } else if (convert_ram_addr_to_num(arg, num, &ram_arg_type) == SPU_OK) {
    return SPU_ADDR_TYPE | ram_arg_type;
  }

  fprintf(stderr, RED("SyntaxError: Can't read neither register nor ram address after pop\n"));

  return SPU_NONE_ARG_TYPE;
}

cmd_code_t handle_jmp_args  (const StrLine *line, int *num, const SpuLabels_t *labels) {
  IOG_ASSERT(line);
  IOG_ASSERT(num);

  char label_name[MAX_LABEL_NAME];
  char cmd[MAX_WORD_NAME];

  if (sscanf(line->line, "%s %s", cmd, label_name) == 1) {
    fprintf(stderr, RED("SyntaxError: Can't find label after jmp\n"));
    return SPU_NONE_ARG_TYPE;
  }

  //fprintf(stderr, RED("[LABEL] ") "%s\n", label_name);
  SpuLabel_t *label = spu_find_label(labels, label_name);
  if (label != NULL) {
    *num = label->addr;
    //fprintf(stderr, RED("[LABEL FOUND] ") "%s\n", label->name);
    return SPU_DATA_TYPE;
  }

  return SPU_NONE_ARG_TYPE;
}

static SpuReturnCode convert_reg_name_to_num (const char *reg_name, int *num) {
  IOG_ASSERT(reg_name);
  IOG_ASSERT(num);

  if ( !((reg_name[0] >= 'a') && (reg_name[0] <= 'z') && (reg_name[1] == 'x')) ) {
    return ERR_NOT_REGISTER;
  }

  *num = (int) (reg_name[0] - 'a');

  return SPU_OK;
}

static SpuReturnCode convert_char_to_num (const char *chr_str, int *num) {
  IOG_ASSERT(chr_str);
  IOG_ASSERT(num);

  size_t chr_len = strlen(chr_str);

  if (chr_len < 3) {
    return SPU_ERR;
  }

  if ((chr_str[0] != '\'') || (chr_str[chr_len-1] != '\'') || (chr_len > 4)) {
    return SPU_ERR;
  }

  if ((chr_str[1] != '\\') && (chr_len == 3)) {
    *num = (int) chr_str[1];
  } else if (strcmp(chr_str, "'\\n'") == 0) {
    *num = (int) '\n';
  } else {
    fprintf(stderr, RED("SyntaxError: Unknown char\n"));
    return SPU_ERR;
  }

  return SPU_OK;
}

static SpuReturnCode convert_ram_addr_to_num (const char *addr, int *num, cmd_code_t *arg_type) {
  IOG_ASSERT(addr);
  IOG_ASSERT(num);

  size_t addr_len = strlen(addr);

  if ((addr[0] != '[') || (addr[addr_len-1] != ']')) {
    return SPU_ERR;
  } 
  
  char num_str[MAX_ARG_NAME] = {};
  strncpy(num_str, addr+1, addr_len-2);

  char reg_name[MAX_REG_NAME] = {};

  if (sscanf(num_str, "%d", num) == 1) {
    *arg_type = SPU_NONE_ARG_TYPE;
    return SPU_OK;
  } else if (sscanf(num_str, "%s", reg_name) == 1) {
    if (convert_reg_name_to_num(reg_name, num) == SPU_OK) {
      *arg_type = SPU_REG_TYPE;
      return SPU_OK;
    }
  }

  return SPU_ERR;
}

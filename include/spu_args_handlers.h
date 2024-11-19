#ifndef SPU_ARGS_HANDLERS_H
#define SPU_ARGS_HANDLERS_H

#include "spu_return_codes.h"
#include "spu_cmd_ids.h"
#include "spu_text_reader.h"
#include "spu_label.h"

typedef cmd_code_t (*ArgsHandler_t) (const StrLine *line, int *num, const SpuLabels_t *labels);

cmd_code_t handle_push_args (const StrLine *line, int *num, const SpuLabels_t *labels);
cmd_code_t handle_pop_args  (const StrLine *line, int *num, const SpuLabels_t *labels);
cmd_code_t handle_jmp_args  (const StrLine *line, int *num, const SpuLabels_t *labels);

static SpuReturnCode convert_reg_name_to_num (const char *reg_name, int *num);
static SpuReturnCode convert_char_to_num     (const char *chr_str,  int *num);
static SpuReturnCode convert_ram_addr_to_num (const char *addr,     int *num, cmd_code_t *arg_type);

#endif // SPU_ARGS_HANDLERS_H

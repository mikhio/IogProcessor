#ifndef SPU_ARGS_HANDLERS_H
#define SPU_ARGS_HANDLERS_H

#include "spu_return_codes.h"
#include "spu_cmd_ids.h"
#include "spu_text_reader.h"

typedef cmd_code_t (*ArgsHandler_t) (StrLine *line, int *num);

cmd_code_t handle_push_args (StrLine *line, int *num);
cmd_code_t handle_pop_args  (StrLine *line, int *num);

static SpuReturnCode convert_reg_name_to_num (char *reg_name, int *num);

#endif // SPU_ARGS_HANDLERS_H

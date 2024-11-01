#ifndef SPU_COMMANDS_H
#define SPU_COMMANDS_H

#include "spu_cmd_ids.h"
#include "spu_args_handlers.h"

struct SpuCmd_t {
  const char         *name;
  const SpuCmdId_t   id;
  const int          argsNum; 
  const ArgsHandler_t handle_args;
};

const int SPU_CMDS_AMOUNT = 6;

const SpuCmd_t SPU_CMDS[SPU_CMDS_AMOUNT] = {
  {"push", SPU_PUSH_ID, 1, handle_push_args},
  {"pop",  SPU_POP_ID,  1, handle_pop_args},
  {"add",  SPU_ADD_ID,  0, NULL},
  {"mul",  SPU_MUL_ID,  0, NULL},
  {"out",  SPU_OUT_ID,  0, NULL},
  {"hlt",  SPU_HLT_ID,  0, NULL},
};

#endif // SPU_COMMANDS_H

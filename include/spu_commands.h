#ifndef SPU_COMMANDS_H
#define SPU_COMMANDS_H

#include "spu_cmd_ids.h"
#include "spu_args_handlers.h"

struct SpuCmd_t {
  const char             *name;
  const SpuCmdId_t       id;
  const int              argsNum; 
  const ArgsHandler_t    handle_args;
  const int              isJump;
};

const int SPU_CMDS_AMOUNT = 7;

const SpuCmd_t SPU_CMDS[SPU_CMDS_AMOUNT] = {
  {"push", SPU_PUSH_ID, 1, handle_push_args, 0},
  {"pop",  SPU_POP_ID,  1, handle_pop_args,  0},
  {"jmp",  SPU_JMP_ID,  1, handle_jmp_args,  1},
  {"add",  SPU_ADD_ID,  0, NULL,             0},
  {"mul",  SPU_MUL_ID,  0, NULL,             0},
  {"out",  SPU_OUT_ID,  0, NULL,             0},
  {"hlt",  SPU_HLT_ID,  0, NULL,             0},
};

#endif // SPU_COMMANDS_H

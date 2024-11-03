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

const int SPU_CMDS_AMOUNT = 15;

const SpuCmd_t SPU_CMDS[SPU_CMDS_AMOUNT] = {
  {"push",  SPU_PUSH_ID, 1, handle_push_args, 0}, // 1
  {"pop",   SPU_POP_ID,  1, handle_pop_args,  0}, // 2

  {"jmp",   SPU_JMP_ID,  1, handle_jmp_args,  1}, // 3
  {"ja",    SPU_JA_ID,   1, handle_jmp_args,  1}, // 4
  {"jae",   SPU_JAE_ID,  1, handle_jmp_args,  1}, // 5
  {"jb",    SPU_JB_ID,   1, handle_jmp_args,  1}, // 6
  {"jbe",   SPU_JBE_ID,  1, handle_jmp_args,  1}, // 7
  {"je",    SPU_JE_ID,   1, handle_jmp_args,  1}, // 8
  {"jne",   SPU_JNE_ID,  1, handle_jmp_args,  1}, // 9
  
  {"call",  SPU_CALL_ID, 1, handle_jmp_args,  1}, // 10
  {"ret",   SPU_RET_ID,  0, NULL,             0}, // 11

  {"add",   SPU_ADD_ID,  0, NULL,             0}, // 12
  {"mul",   SPU_MUL_ID,  0, NULL,             0}, // 13

  {"out",   SPU_OUT_ID,  0, NULL,             0}, // 14
  {"hlt",   SPU_HLT_ID,  0, NULL,             0}, // 15
};

#endif // SPU_COMMANDS_H

#ifndef SPU_CMD_IDS_H
#define SPU_CMD_IDS_H

enum SpuCmdId_t {
  SPU_NONE_ID  = 0,

  SPU_PUSH_ID  = 1,
  SPU_ADD_ID   = 2,
  SPU_OUT_ID   = 3,
  SPU_HLT_ID   = 4,
  SPU_MUL_ID   = 5,

  SPU_POP_ID   = 6,
  SPU_PUSHR_ID = 7,

  SPU_JMP_ID   = 8,
  SPU_JA_ID    = 9,
  SPU_JAE_ID   = 10,
  SPU_JB_ID    = 11,
  SPU_JBE_ID   = 12,
  SPU_JE_ID    = 13,
  SPU_JNE_ID   = 14,
};

#endif // SPU_CMD_IDS_H

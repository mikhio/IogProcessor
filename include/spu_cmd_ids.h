#ifndef SPU_CMD_IDS_H
#define SPU_CMD_IDS_H

typedef char cmd_code_t;


const cmd_code_t SPU_CMD_ID_MSK    = 0b00011111;
const cmd_code_t SPU_ARG_MSK       = 0b11100000;

const cmd_code_t SPU_NONE_ARG_TYPE = 0b00000000;
const cmd_code_t SPU_DATA_TYPE     = 0b00100000;
const cmd_code_t SPU_REG_TYPE      = 0b01000000;
const cmd_code_t SPU_ADDR_TYPE     = 0b10000000;

#define GET_CMD_CODE(arg_type, cmd_id) ( (arg_type & SPU_ARG_MSK) | (cmd_id & SPU_CMD_ID_MSK) )
#define GET_ARG_TYPE(cmd_code)         ( (cmd_code & SPU_ARG_MSK) )
#define GET_CMD_ID(cmd_code)           ( (cmd_code & SPU_CMD_ID_MSK) )

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
  
  SPU_CALL_ID  = 15,
  SPU_RET_ID   = 16,

  SPU_SUB_ID   = 17,
  SPU_DIV_ID   = 18,
};

#endif // SPU_CMD_IDS_H

#ifndef SPU_RETURN_CODES_H
#define SPU_RETURN_CODES_H

enum SpuReturnCode {
  SPU_OK               = 0,
  SPU_QUIT             = 1,
  ERR_MANY_ARGUMENTS   = 2,
  ERR_CANT_OPEN_FILE   = 3,
  ERR_CANT_CODE_APPEND = 4,
  ERR_CANT_SAVE_CODE   = 5,

  NR_SPU_RETURN_CODE   = 6,
};


#endif // SPU_RETURN_CODES_H

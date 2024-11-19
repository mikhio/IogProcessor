#ifndef SPU_RETURN_CODES_H
#define SPU_RETURN_CODES_H

enum SpuReturnCode {
  SPU_OK               = 0,
  SPU_QUIT             = 1,

  ERR_MANY_ARGUMENTS   = 2,
  ERR_CANT_OPEN_FILE   = 3,
  ERR_CANT_APPEND      = 4,
  ERR_CANT_SAVE_CODE   = 5,
  ERR_NOT_REGISTER     = 6,
  ERR_CANT_INIT        = 7,

  ERR_CANT_ALLOCATE    = 8,
  ERR_CANT_READ_FILE   = 9,
  ERR_CANT_SPLIT       = 10,
  ERR_NULL_BUFFER      = 11,
  ERR_NO_OUTPUT_FILE   = 12,
  ERR_CANT_FILL_BUFFER = 13,

  ERR_CANT_HANDLE_ARG  = 14,
  
  ERR_INCORRECT        = 15,

  SPU_ERR              = 16,

  NR_SPU_RETURN_CODE   = 17,
};


#endif // SPU_RETURN_CODES_H

#ifndef _LOGGER_ASM_H_
#define _LOGGER_ASM_H_

#include <stdio.h>

extern const char* input_path_name;

#define ReportError(...)                      \
  do{                                         \
    extern int row, column;                   \
    extern int semantic_errors;               \
                                              \
    semantic_errors = 1;                      \
    printf("%s:%d:%d: error: ",               \
      input_path_name, row, column);          \
    printf(__VA_ARGS__);                      \
    printf("\n");                             \
  } while(0)

#define ReportWarning(...)                    \
  do{                                         \
    extern int row, column;                   \
                                              \
    printf("%s:%d:%d: warning: ",             \
      input_path_name, row, column);          \
    printf(__VA_ARGS__);                      \
    printf("\n");                             \
  } while(0)

#endif
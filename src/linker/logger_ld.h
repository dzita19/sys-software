#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>

#define ReportError(...)                    \
  do{                                       \
    extern int link_errors;                 \
                                            \
    link_errors = 1;                        \
    printf("ERROR: " __VA_ARGS__);          \
    printf("\n");                           \
  } while(0)

#define ReportWarning(...)                  \
  do{                                       \
    printf("WARNING: " __VA_ARGS__);        \
    printf("\n");                           \
  } while(0)

#endif
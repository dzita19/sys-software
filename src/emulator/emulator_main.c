#include "emulator.h"
#include <stdio.h>
#include <string.h>

extern FILE* emuin;

#define CMD_PARSE_ERROR (-1)
#define CMD_PARSE_INPUT   0
#define CMD_PARSE_ARGS    1

static int         parse_mode    = CMD_PARSE_INPUT;
static const char* input_path    = 0;
int                debug_mode    = 0;

static void ParseCmdArgs(int argc, char** argv){
  for(int i = 1; i < argc; i++){
    if(parse_mode == CMD_PARSE_ERROR) break;

    switch(parse_mode){
    case CMD_PARSE_INPUT:
      if     (strcmp(argv[i], "-dbg-short") == 0) debug_mode = DEBUG_MODE_SHORT;
      else if(strcmp(argv[i], "-dbg-long")  == 0) debug_mode = DEBUG_MODE_LONG;
      else if(strcmp(argv[i], "-args") == 0) {
        parse_mode = CMD_PARSE_ARGS;
        exec_args = VectorInit();
        VectorPush(exec_args, (char*)input_path);
      }
      else {
        if(input_path == 0) input_path = argv[i];
        else parse_mode = CMD_PARSE_INPUT;
      }
      break;
    case CMD_PARSE_ARGS:
      VectorPush(exec_args, argv[i]);
      break;
    }
  }
}

int main(int argc, char* argv[]){
  
  ParseCmdArgs(argc, argv);

  if(input_path == 0){
    parse_mode = CMD_PARSE_ERROR;
  }

  if(parse_mode == CMD_PARSE_ERROR){
    printf("Invalid arguments.\n");
    printf("Recommended input: <input_file> [*flags*] [-args {args}]\n"
           "\tFlags:          -dbg-short, -dbg-long (debug outputs)\n");
    return 1;
  }

  emuin = fopen(input_path, "r");
  if(emuin == NULL){
    printf("Input file cannot be found.");
    return 1;
  }

  EmulateProgram();
  
  fclose(emuin);
  return 0;
}
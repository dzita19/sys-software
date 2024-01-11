#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linker.h"
#include "util/memory_safety.h"

extern FILE* ldout;

#define CMD_PARSE_ERROR (-1)
#define CMD_PARSE_INPUT   0
#define CMD_PARSE_OUTPUT  1

static const char*  output_path   = 0;
static const char** input_paths   = 0;
static int          input_size    = 0;
static int          parse_state   = CMD_PARSE_INPUT;
static int          debug_enabled = 0;

static void ParseCmdArgs(int argc, char** argv){
  for(int i = 1; i < argc; i++){
    switch(parse_state){
    case CMD_PARSE_ERROR: break;

    case CMD_PARSE_INPUT:{
      if(strcmp(argv[i], "-o") == 0){
        parse_state = CMD_PARSE_OUTPUT;
      }
      else if(strcmp(argv[i], "-dbg") == 0){
        debug_enabled = 1;
      }
      else{
        input_paths[input_size++] = argv[i];
      }
    } break;

    case CMD_PARSE_OUTPUT:{
      if     (strcmp(argv[i], "-o")   == 0) parse_state = CMD_PARSE_ERROR;
      else if(strcmp(argv[i], "-dbg") == 0) parse_state = CMD_PARSE_ERROR;
      else{
        if(output_path == 0) {
          output_path = argv[i];
          parse_state = CMD_PARSE_INPUT;
        }
        else parse_state = CMD_PARSE_ERROR;
      }
    } break;
    }
  }
}

int main(int argc, char** argv){

  input_paths = malloc(sizeof(char*) * argc);

  ParseCmdArgs(argc, argv);

  if(input_size == 0 || output_path == 0){
    parse_state = CMD_PARSE_ERROR;
  }

  if(parse_state == CMD_PARSE_ERROR){
    printf("Invalid arguments.\n");
    printf("Recommended input: -o <output_file> {<input_file>} [*flags*]\n"
           "\tFlags:          -dbg (debug output)\n");
    free(input_paths);
    return 1;
  }

  LinkerInit();

  // if debug enabled, last arg is not input file name
  for(int i = 0; i < input_size; i++){
    FILE* input_file = fopen(input_paths[i], "r");
    if(input_file == NULL) {
      printf("Input file not found.\n");
      return 1;
    }

    LinkerAssignFile(input_file);
    fclose(input_file);
  }

  LinkerLinkData();
  if(link_errors == 0){
    ldout = fopen(output_path, "w");
    LinkerGenerateOutput();
  }

  if(debug_enabled) {
    LinkerPrint();
    printf("\n");
  }

  LinkerFree();
  if(ldout) fclose(ldout);

  if(debug_enabled){
    memory_safety_report();
  }

  free(input_paths);

  return 0;
}
#include <stdio.h>
#include <string.h>

#include "util/memory_safety.h"
#include "assembler.h"
#include "code_gen.h"

extern FILE* yyin;
extern FILE* asout;
extern int yyparse();

extern const char* input_path_name;

#define CMD_PARSE_ERROR (-1)
#define CMD_PARSE_INPUT   0
#define CMD_PARSE_OUTPUT  1

static const char* input_path    = 0;
static const char* output_path   = 0;
static int         parse_state   = CMD_PARSE_INPUT;
static int         debug_enabled = 0;

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
        if(input_path == 0) input_path = argv[i];
        else parse_state = CMD_PARSE_ERROR;
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
  
  ParseCmdArgs(argc, argv);

  if(input_path == 0 || output_path == 0){
    parse_state = CMD_PARSE_ERROR;
  }

  if(parse_state != CMD_PARSE_INPUT) {
    printf("Invalid arguments.\n");
    printf("Recommended input: -o <output_file> <input_file> [*flags*]\n"
           "\tFlags:          -dbg (debug output)\n");
    return 1;
  }
  
  input_path_name = input_path;
  yyin  = fopen(input_path, "r");

  if(yyin == 0) {
    printf("Input file not found.\n");
    return 1;
  }

  AssemblerInit();
  if(yyparse() == 0){
    if(debug_enabled) printf("Parsing finished successfully.\n");

    if(semantic_errors == 0) {
      ResolveGlobalSymbols();
      ResolveDefSymbols();
      if(semantic_errors == 0) {
        asout = fopen(output_path, "w");
        GenerateMachineCode();
      }
    }
  }
  else {
    if(debug_enabled) printf("Parsing failed.\n");
  }

  fclose(yyin);
  if(asout) fclose(asout);

  if(debug_enabled) {
    AssemblerPrint();
    printf("\n");
  }
  
  AssemblerFree();

  if(debug_enabled) memory_safety_report();

  return 0;
}
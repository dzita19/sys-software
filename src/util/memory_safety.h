#ifndef _MEMORY_SAFETY_
#define _MEMORY_SAFETY_

// objdata
extern int symtab_entry_alloc;
extern int symtab_entry_free;
extern int symtab_alloc;
extern int symtab_free;
extern int section_alloc;
extern int section_free;
extern int relatab_entry_alloc;
extern int relatab_entry_free;
extern int relatab_alloc;
extern int relatab_free;
extern int obj_data_alloc;
extern int obj_data_free;

// assembler
extern int asm_expr_alloc;
extern int asm_expr_free;
extern int asm_operand_alloc;
extern int asm_operand_free;
extern int asm_dir_alloc;
extern int asm_dir_free;
extern int asm_instr_alloc;
extern int asm_instr_free;
extern int asm_line_alloc;
extern int asm_line_free;

// linker
extern int global_def_alloc;
extern int global_def_free;

// util
extern int linked_list_alloc;
extern int linked_list_free;
extern int node_alloc;
extern int node_free;
extern int string_alloc;
extern int string_free;
extern int vector_alloc;
extern int vector_free;
extern int byte_vector_alloc;
extern int byte_vector_free;
extern int array_alloc;
extern int array_free;

extern char* StringDuplicate();
extern char* StringAllocate(int size);
extern void  StringDrop(const char*);

extern void memory_safety_report();

#endif
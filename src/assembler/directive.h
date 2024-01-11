#ifndef _DIRECTIVE_H_
#define _DIRECTIVE_H_

typedef enum AsmDirective{
  DIR_ERROR,
  DIR_END,
  DIR_SECTION,
  DIR_GLOBL,
  DIR_EXTRN,
  DIR_DEF,
  DIR_BYTE,
  DIR_WORD,
  DIR_LONG,
  DIR_ZERO,
  DIR_SKIP,
  DIR_ASCIZ,
  DIR_NOTYP,
  DIR_FUNCT,
  DIR_OBJCT,
} AsmDirective;

extern const char* dir_names[];

extern void SymbolText(void);  
extern void SymbolData(void);  
extern void SymbolRodata(void);

extern void End          (void);
extern void SectionSymbol(void);
extern void SectionText  (void);
extern void SectionData  (void);
extern void SectionRodata(void);
extern void SectionText  (void);
extern void SectionData  (void);
extern void SectionRodata(void);
extern void Global       (void);
extern void Extern       (void);
extern void Define       (void);
extern void ByteData     (void);
extern void WordData     (void);
extern void LongData     (void);
extern void ZeroData     (void);
extern void SkipData     (void);
extern void Asciiz       (void);
extern void NoType       (void);
extern void Function     (void);
extern void Object       (void);

#endif
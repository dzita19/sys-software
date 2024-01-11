#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

extern void SymbolToInstr(void);

extern void NullaryInstr(void);
extern void UnaryRegInstr(void);
extern void UnaryOpInstr(void);
extern void BinaryRegToRegInstr(void);
extern void BinaryOpToRegInstr(void);
extern void BinaryRegToOpInstr(void);

extern void OperandRegdir(void);
extern void OperandRegdirConstAdd(void);
extern void OperandRegdirConstSub(void);

extern void OperandImmedConstPlus(void);
extern void OperandImmedConstMinus(void);
extern void OperandImmedSymbol(void);
extern void OperandImmedSymbolConstAdd(void);
extern void OperandImmedSymbolConstSub(void);

extern void OperandRegind(void);
extern void OperandRegindConstAdd(void);
extern void OperandRegindConstSub(void);

extern void OperandMemdirConstPlus(void);
extern void OperandMemdirConstMinus(void);
extern void OperandMemdirSymbol(void);
extern void OperandMemdirSymbolConstAdd(void);
extern void OperandMemdirSymbolConstSub(void);

#endif
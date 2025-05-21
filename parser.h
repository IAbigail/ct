#pragma once

#include <stdbool.h>
#include "lexer.h"  // to get Token definition and token codes

extern Token *tokens;  // token list head, defined elsewhere
extern Token *crtTk;   // current token pointer, defined in parser.c

// Parsing functions
void parse(void);
void unit(void);

bool consume(int code);
void parseError(const char *msg);

bool declStruct(void);
bool declFunc(void);
bool declVar(void);

bool typeBase(void);
bool arrayDecl(void);
bool typeName(void);

bool stmCompound(void);
bool stm(void);

bool expr(void);
bool exprAssign(void);
bool exprUnary(void);
bool exprOr(void);
bool exprAnd(void);
bool exprEq(void);
bool exprRel(void);
bool exprAdd(void);
bool exprMul(void);
bool exprCast(void);
bool exprPrimary(void);
bool exprPostfix(void);

bool funcArg(void);

void expect(int code, const char *msg);  // This function is called but not defined in the code you shared; you might need to implement it or declare it if defined elsewhere



#pragma once

// Token type codes
enum
{
    STRUCT, INT, DOUBLE, CHAR, VOID, IF, ELSE, WHILE, FOR, BREAK, RETURN,
    ASSIGN, ADD, SUB, MUL, DIV, AND, OR, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ,
    SEMICOLON, COMMA, LPAR, RPAR, LACC, RACC, LBRACKET, RBRACKET,
    CT_INT, CT_REAL, CT_CHAR, CT_STRING,
    ID, END,
    ANDAND,    // &&
    OROR,      // ||
    NOT, DOT     // !
};

// Token structure
typedef struct _Token
{
    int code;
    union
    {
        char *text;   // Used for ID, CT_STRING, CT_CHAR
        long int i;   // Used for integer constants
        double r;     // Used for real constants
    };
    int line;
    struct _Token *next;
} Token;

extern Token *tokens;

Token *getNextToken(void);

void showTokens(void);
void tokenizeFile(FILE *f);



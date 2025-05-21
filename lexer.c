#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "include/lexer.h"


Token *tokens = NULL, *lastToken = NULL;
char *pCrtCh; // Pointer to current character
int line = 1;

#define SAFEALLOC(var, Type)                          \
    if ((var = (Type *)malloc(sizeof(Type))) == NULL) \
        err("Not enough memory");

void err(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "fatal error: ");
    vfprintf(stderr, fmt, va);
    va_end(va);
    exit(-1);
}

void tkerr(Token *tk, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fprintf(stderr, " at line %d\n", tk->line);
    va_end(va);
    exit(-1);
}

Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk, Token)
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastToken)
    {
        lastToken->next = tk;
    }
    else
    {
        tokens = tk;
    }
    lastToken = tk;
    
    return tk;
}

int getNextKeywordOrIDToken()
{
    char ch = *pCrtCh;
    const char *pStartCh;
    Token *tk;

    if (isalpha(ch) || ch == '_')
    {
        pStartCh = pCrtCh;
        pCrtCh++;
        while (isalnum(*pCrtCh) || *pCrtCh == '_')
        {
            pCrtCh++;
        }

        char *word = strndup(pStartCh, pCrtCh - pStartCh);

        if (strcmp(word, "struct") == 0)
        {
            addTk(STRUCT);
        }
        else if (strcmp(word, "int") == 0)
        {
            addTk(INT);
        }
        else if (strcmp(word, "double") == 0)
        {
            addTk(DOUBLE);
        }
        else if (strcmp(word, "char") == 0)
        {
            addTk(CHAR);
        }
        else if (strcmp(word, "void") == 0)
        {
            addTk(VOID);
        }
        else if (strcmp(word, "if") == 0)
        {
            addTk(IF);
        }
        else if (strcmp(word, "else") == 0)
        {
            addTk(ELSE);
        }
        else if (strcmp(word, "while") == 0)
        {
            addTk(WHILE);
        }
        else if (strcmp(word, "for") == 0)
        {
            addTk(FOR);
        }
        else if (strcmp(word, "break") == 0)
        {
            addTk(BREAK);
        }
        else if (strcmp(word, "return") == 0)
        {
            addTk(RETURN);
        }
        else
        {
            tk = addTk(ID);
            tk->text = word;
        }
        return 1;
    }
    return 0;
}
int getNextOperatorToken()
{
    char ch = *pCrtCh;
    Token *tk;

    switch (ch)
    {
        case '=':
            pCrtCh++;
            if (*pCrtCh == '=')
            {
                pCrtCh++;
                addTk(EQUAL);
                return EQUAL; // ==
            }
            else
            {
                addTk(ASSIGN);
                return ASSIGN; // =
            }

        case '+':
            pCrtCh++;
            addTk(ADD);
            return ADD;

        case '-':
            pCrtCh++;
            addTk(SUB);
            return SUB;

        case '*':
            pCrtCh++;
            addTk(MUL);
            return MUL;

        case '/':
            pCrtCh++;
            addTk(DIV);
            return DIV;

        case '&':
            pCrtCh++;
    if (*pCrtCh == '&')
    {
        pCrtCh++;
        addTk(ANDAND);
        return ANDAND;
    }
    else
    {
        addTk(AND);
        return AND;
    }


        case '|':
            pCrtCh++;
            if (*pCrtCh == '|')
            {
                pCrtCh++;
                addTk(OROR);  // ||
                return OROR;
            }
            else
            {
                addTk(OR); // single |
                return OR;
            }

        case '!':
            pCrtCh++;
            if (*pCrtCh == '=')
            {
                pCrtCh++;
                addTk(NOTEQ);  // !=
                return NOTEQ;
            }
            else
            {
                addTk(NOT);  // !
                return NOT;
            }

        case '<':
            pCrtCh++;
            if (*pCrtCh == '=')
            {
                pCrtCh++;
                addTk(LESSEQ);  // <=
                return LESSEQ;
            }
            else
            {
                addTk(LESS);  // <
                return LESS;
            }

        case '>':
            pCrtCh++;
            if (*pCrtCh == '=')
            {
                pCrtCh++;
                addTk(GREATEREQ);  // >=
                return GREATEREQ;
            }
            else
            {
                addTk(GREATER);  // >
                return GREATER;
            }

        default:
            return -1;
    }
}

int getNextDelimiterToken()
{
    char ch = *pCrtCh;

    switch (ch)
    {
        case ';':
            pCrtCh++;
            addTk(SEMICOLON);
            return SEMICOLON;
        case ',':
            pCrtCh++;
            addTk(COMMA);
            return COMMA;
        case '(':
            pCrtCh++;
            addTk(LPAR);
            return LPAR;
        case ')':
            pCrtCh++;
            addTk(RPAR);
            return RPAR;
        case '{':
            pCrtCh++;
            addTk(LACC);
            return LACC;
        case '}':
            pCrtCh++;
            addTk(RACC);
            return RACC;
        case '[':
            pCrtCh++;
            addTk(LBRACKET);
            return LBRACKET;
        case ']':
            pCrtCh++;
            addTk(RBRACKET);
            return RBRACKET;
        default:
            return -1;
    }

}
Token *getNextConstantToken()
{
    char ch = *pCrtCh;
    const char *pStartCh;
    Token *tk;

    if ((isdigit(ch)) || (ch == '.' && isdigit(*(pCrtCh + 1)))) 
    {
        pStartCh = pCrtCh;

        // Integer part
        while (isdigit(*pCrtCh)) pCrtCh++;

        // Fractional part
        if (*pCrtCh == '.') {
            pCrtCh++;
            while (isdigit(*pCrtCh)) pCrtCh++;
        }

        // Exponent part
        if (*pCrtCh == 'e' || *pCrtCh == 'E') {
            pCrtCh++; // skip 'e' or 'E'

            if (*pCrtCh == '+' || *pCrtCh == '-') pCrtCh++; // optional sign

            if (!isdigit(*pCrtCh)) {
                tkerr(addTk(END), "invalid floating point exponent");
                return NULL;
            }

            while (isdigit(*pCrtCh)) pCrtCh++;
        }

        // Check if real number (float) or int
        int isReal = 0;
        for (const char *p = pStartCh; p < pCrtCh; p++) {
            if (*p == '.' || *p == 'e' || *p == 'E') {
                isReal = 1;
                break;
            }
        }

        tk = addTk(isReal ? CT_REAL : CT_INT);
        int len = pCrtCh - pStartCh;
        char *text = strndup(pStartCh, len);

        if (isReal) {
            tk->r = strtod(text, NULL);
            tk->text = text; // Keep original float string
        } else {
            tk->i = strtol(text, NULL, 10);
            free(text); // No need to keep int string
        }

        return tk;
    }

    // Char constant
    if (ch == '\'') {
        pStartCh = pCrtCh++;
        if (*pCrtCh == '\\') pCrtCh += 2; // escape sequence like '\n'
        else pCrtCh++; // normal char

        if (*pCrtCh != '\'') {
            tkerr(addTk(END), "missing closing ' for char constant");
            return NULL;
        }
        pCrtCh++; // skip closing '

        tk = addTk(CT_CHAR);
        tk->text = strndup(pStartCh + 1, pCrtCh - pStartCh - 2); // exclude quotes
        return tk;
    }

    // String constant
    if (ch == '"') {
        pStartCh = pCrtCh++;
        while (*pCrtCh && *pCrtCh != '"') {
            if (*pCrtCh == '\\' && *(pCrtCh + 1)) pCrtCh += 2; // escape sequence
            else pCrtCh++;
        }

        if (*pCrtCh != '"') {
            tkerr(addTk(END), "missing closing \" for string constant");
            return NULL;
        }
        pCrtCh++; // skip closing "

        tk = addTk(CT_STRING);
        tk->text = strndup(pStartCh + 1, pCrtCh - pStartCh - 2); // exclude quotes
        return tk;
    }

    // Not a constant token
    return NULL;
}


void skipComments()
{
    if (*pCrtCh == '/')
    {
        if (*(pCrtCh + 1) == '/') // single-line comment
        {
            pCrtCh += 2;
            while (*pCrtCh != '\n' && *pCrtCh != '\0')
                pCrtCh++;
        }
        else if (*(pCrtCh + 1) == '*') // block comment
        {
            pCrtCh += 2;
            while (*pCrtCh != '\0')
            {
                if (*pCrtCh == '*' && *(pCrtCh + 1) == '/')
                {
                    pCrtCh += 2;
                    break;
                }
                if (*pCrtCh == '\n')
                    line++;
                pCrtCh++;
            }
        }
    }
}

Token *getNextToken()
{
    while (1)
    {
        char ch = *pCrtCh;

        // Skip whitespace characters
        if (ch == ' ' || ch == '\t' || ch == '\r')
        {
            pCrtCh++;
            continue;
        }
        if (ch == '\n')
        {
            line++;
            pCrtCh++;
            continue;
        }
        // Skip comments if present
         if (ch == '/')
         {
        if (*(pCrtCh + 1) == '/' || *(pCrtCh + 1) == '*')
        {
            skipComments();
            continue;
        }
         }

        // Check for keywords and identifiers
        if (getNextKeywordOrIDToken())
            continue;

        // Check for operators
        if (getNextOperatorToken() != -1)
            continue;

        // Check for delimiters
        if (getNextDelimiterToken() != -1)
            continue;

        // Check for constants
        if (getNextConstantToken() != NULL)
            continue;

        // Check for end of input string
       if (ch == 0 || *pCrtCh == '\0')  
        {
             Token *endToken = addTk(END);  // Add the END token to indicate the end of input
             return endToken;               // Return pointer to END token
        }

        // Handle invalid character
        tkerr(addTk(END), "invalid character");  // Report the invalid character error
        break;  // Exit the loop as we've encountered an invalid character
    }
     return addTk(END);
}

void showTokens()
{
    Token *tk = tokens;
    while (tk)
    {
        switch (tk->code)
        {
            case STRUCT: printf("STRUCT\n"); break;
            case INT: printf("INT\n"); break;
            case DOUBLE: printf("DOUBLE\n"); break;
            case CHAR: printf("CHAR\n"); break;
            case VOID: printf("VOID\n"); break;
            case IF: printf("IF\n"); break;
            case ELSE: printf("ELSE\n"); break;
            case WHILE: printf("WHILE\n"); break;
            case FOR: printf("FOR\n"); break;
            case BREAK: printf("BREAK\n"); break;
            case RETURN: printf("RETURN\n"); break;

            case ASSIGN: printf("ASSIGN\n"); break;
            case ADD: printf("ADD\n"); break;
            case SUB: printf("SUB\n"); break;
            case MUL: printf("MUL\n"); break;
            case DIV: printf("DIV\n"); break;
            case AND: printf("AND\n"); break;
            case OR: printf("OR\n"); break;
            case EQUAL: printf("EQUAL\n"); break;
            case NOTEQ: printf("NOTEQ\n"); break;
            case LESS: printf("LESS\n"); break;
            case LESSEQ: printf("LESSEQ\n"); break;
            case GREATER: printf("GREATER\n"); break;
            case GREATEREQ: printf("GREATEREQ\n"); break;

            case SEMICOLON: printf("SEMICOLON\n"); break;
            case COMMA: printf("COMMA\n"); break;
            case LPAR: printf("LPAR\n"); break;
            case RPAR: printf("RPAR\n"); break;
            case LACC: printf("LACC\n"); break;
            case RACC: printf("RACC\n"); break;
            case LBRACKET: printf("LBRACKET\n"); break;
            case RBRACKET: printf("RBRACKET\n"); break;
            case ANDAND: printf("ANDAND\n"); break;
            case OROR:   printf("OROR\n"); break;
            case NOT:    printf("NOT\n"); break;


            case CT_INT: printf("CT_INT: %ld\n", tk->i); break;
          case CT_REAL: printf("CT_REAL: %s\n", tk->text); break;
            case CT_CHAR: printf("CT_CHAR: %s\n", tk->text); break;
            case CT_STRING: printf("CT_STRING: %s\n", tk->text); break;

            case ID: printf("ID: %s\n", tk->text); break;
            case END: printf("END\n"); break;
        }
        tk = tk->next;
    }
}void tokenizeFile(FILE *f)
{
    const char *inputFile = "tests/testparser.c";
    FILE *file = fopen(inputFile, "r");
    if (!file)
    {
        perror("Error opening input file");
        exit(EXIT_FAILURE); // Add exit to stop on failure
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET); 

    // Allocate buffer and read the whole file
    char *source = malloc(fileSize + 1);
    if (!source)
    {
        perror("Failed to allocate memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(source, 1, fileSize, file);
    source[fileSize] = '\0';  // Null-terminate the source code string
    fclose(file);

    // Initialize global lexer state
    pCrtCh = source;
    line = 1;
    tokens = NULL;
    lastToken = NULL;

    // Tokenize and build linked list of tokens
    Token *tk;
    do {
        tk = getNextToken();
        Token *copy = malloc(sizeof(Token));
        *copy = *tk;  // shallow copy is fine if you're not duplicating text
        copy->next = NULL;

        if (lastToken) {
            lastToken->next = copy;
        } else {
            tokens = copy;
        }
        lastToken = copy;
    } while (tk->code != END);

    // Show all tokens
    showTokens();

    // Free original source buffer
    free(source);
    

    // NOTE: Don't free the tokens here. Let the parser use them first!
    // Instead, free them AFTER parsing in main(), or implement a cleanup function
}

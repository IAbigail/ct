#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

enum
{
    STRUCT, INT, DOUBLE, CHAR, VOID, IF, ELSE, WHILE, FOR, BREAK, RETURN,
    ASSIGN, ADD, SUB, MUL, DIV, AND, OR, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ,
    SEMICOLON, COMMA, LPAR, RPAR, LACC, RACC, LBRACKET, RBRACKET,
    CT_INT, CT_REAL, CT_CHAR, CT_STRING,
    ID, END
};

typedef struct _Token
{
    int code;
    union
    {
        char *text; // Used for ID, CT_STRING, CT_CHAR
        long int i; // Used for integers
        double r;   // Used for real numbers
    };
    int line;
    struct _Token *next;
} Token;

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
                return EQUAL;
            }
            else
            {
                addTk(ASSIGN);
                return ASSIGN;
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
            addTk(AND);
            return AND;
        case '|':
            pCrtCh++;
            addTk(OR);
            return OR;
        case '!':
            pCrtCh++;
            addTk(NOTEQ);
            return NOTEQ;
        case '<':
            pCrtCh++;
            addTk(LESS);
            return LESS;
        case '>':
            pCrtCh++;
            addTk(GREATER);
            return GREATER;
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

int getNextConstantToken()
{
    char ch = *pCrtCh;
    const char *pStartCh;
    Token *tk;

    if (isdigit(ch))
    {
        pStartCh = pCrtCh;
        pCrtCh++;
        while (isdigit(*pCrtCh))
        {
            pCrtCh++;
        }

        if (*pCrtCh == '.')
        {
            pCrtCh++;
            while (isdigit(*pCrtCh))
            {
                pCrtCh++;
            }
            tk = addTk(CT_REAL);
            tk->r = strtod(pStartCh, NULL);
            return CT_REAL;
        }
        else
        {
            tk = addTk(CT_INT);
            tk->i = strtol(pStartCh, NULL, 10);
            return CT_INT;
        }
    }

    if (ch == '\'')  // Single character constant
    {
        pStartCh = pCrtCh;
        pCrtCh++;
        while (*pCrtCh != '\'' && *pCrtCh != 0)
        {
            pCrtCh++;
        }
        if (*pCrtCh == '\'')
        {
            pCrtCh++;
            tk = addTk(CT_CHAR);
            tk->text = strndup(pStartCh, pCrtCh - pStartCh);
            return CT_CHAR;
        }
    }

    if (ch == '\"')  // String constant
    {
        pStartCh = pCrtCh;
        pCrtCh++;
        while (*pCrtCh != '\"' && *pCrtCh != 0)
        {
            pCrtCh++;
        }
        if (*pCrtCh == '\"')
        {
            pCrtCh++;
            tk = addTk(CT_STRING);
            tk->text = strndup(pStartCh, pCrtCh - pStartCh);
            return CT_STRING;
        }
    }

    return -1;
}

int getNextToken()
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
        if (getNextConstantToken() != -1)
            continue;

        // Check for end of input string
        if (ch == 0 || *pCrtCh == '\0')  
        {
            addTk(END);  // Add the END token to indicate the end of the input
            return END;   // Return END token
        }

        // Handle invalid character
        tkerr(addTk(END), "invalid character");  // Report the invalid character error
        break;  // Exit the loop as we've encountered an invalid character
    }
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

            case CT_INT: printf("CT_INT: %ld\n", tk->i); break;
            case CT_REAL: printf("CT_REAL: %.2f\n", tk->r); break;
            case CT_CHAR: printf("CT_CHAR: %s\n", tk->text); break;
            case CT_STRING: printf("CT_STRING: %s\n", tk->text); break;

            case ID: printf("ID: %s\n", tk->text); break;
            case END: printf("END\n"); break;
        }
        tk = tk->next;
    }
}
int main()
{
    FILE *file = fopen("0.c", "r");  // Open the file 0.c for reading
    if (!file)
    {
        perror("Error opening file");
        return 1;  // Return a non-zero value indicating an error
    }

    // Read the entire content of the file into a string
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *source = (char *)malloc(file_size + 1);
    if (!source)
    {
        perror("Error allocating memory for file content");
        return 1;  // Return a non-zero value indicating an error
    }

    fread(source, 1, file_size, file);
    source[file_size] = '\0';  // Null-terminate the string

    fclose(file);  // Close the file

    pCrtCh = source;

    while (getNextToken() != END)
    {
        // Keep getting tokens until END
    }

    showTokens();  // Display the tokens
    free(source);  // Free the allocated memory for the source

    return 0;  // Return 0 to indicate successful execution
}

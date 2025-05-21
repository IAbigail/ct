#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/lexer.h"
#include "include/parser.h"


Token *crtTk = NULL;  // current token pointer used by parser


void expect(int code, const char *msg) {
    if (!consume(code)) {
        parseError(msg);
    }
}

bool consume(int code) {
    if (crtTk == NULL) {
        parseError("Unexpected end of input");
    }
    if (crtTk->code == code) {
        printf("Consumed: %d at line %d\n", code, crtTk->line);

        
        crtTk = crtTk->next;
        return true;
    }
    return false;
}


void parseError(const char *msg) {
    fprintf(stderr, "Parse error at line %d: %s (at token code %d)\n", crtTk ? crtTk->line : -1, msg, crtTk ? crtTk->code : -1);
    exit(EXIT_FAILURE);
}


// Parsing a unit (top level: structs, functions, variables)
void unit() {
    while (true) {
        if (declStruct()) continue;
        if (declFunc()) continue;
        if (declVar()) continue;
        break;
    }
    if (!consume(END)) {
        parseError("Expected END of input");
    }
}
bool typeBase() {
    if (consume(INT)) {
       
        return true;
    }
    if (consume(DOUBLE)) {
       
        return true;
    }
    if (consume(CHAR)) {
       
        return true;
    }
    if (consume(STRUCT)) {
        if (!consume(ID)) {
            parseError("Expected struct name after 'struct'");
        }
        return true;
    }
    return false;
}

bool arrayDecl() {
    if (!consume(LBRACKET)) 
		return false;
    expr();  // expr optional inside brackets, ignore errors here
    if (!consume(RBRACKET)) 
	{
        parseError("Expected ']' after array declaration");
    }
    return true;
}

bool typeName() {
    if (!typeBase()) return false;
    arrayDecl();  // optional array declaration
    return true;
}

bool declVar() {
    if (!typeBase()) return false;

    if (!consume(ID)) {
        parseError("Expected variable name");
    }
    arrayDecl();

    while (consume(COMMA)) {
        if (!consume(ID)) {
            parseError("Expected variable name after ','");
        }
        arrayDecl();
    }
    if (!consume(SEMICOLON)) {
        parseError("Expected ';' after variable declaration");
    }
    return true;
}


bool declStruct() {
    Token *startTk = crtTk;  // Save current token position for rollback

    if (!consume(STRUCT)) {
        return false;
    }

    if (!consume(ID)) {
        parseError("Expected struct name");
    }

   if (consume(LACC)) {
    // Parse struct members

    while (true) {
    if (crtTk->code == RACC) break;
    if (!declVar()) {
        parseError("Expected variable declaration inside struct");
    }
}
        if (!consume(RACC)) {
            parseError("Expected '}' after struct members");
        }
        if (!consume(SEMICOLON)) {
            parseError("Expected ';' after struct definition");
        }
        return true;
    } else {
        // Parse struct variable declaration(s)
        if (!consume(ID)) {
            parseError("Expected variable name after struct name");
        }
        arrayDecl();

        while (consume(COMMA)) {
            if (!consume(ID)) {
                parseError("Expected variable name after ','");
            }
            arrayDecl();
        }

        if (!consume(SEMICOLON)) {
            parseError("Expected ';' after struct variable declaration");
        }
        return true;
    }

    // If parsing fails, rollback tokens (optional)
    crtTk = startTk;
    return false;
}
bool declFunc() {
    Token *startTk = crtTk;  // save current token for rollback

    // ✅ Parse return type using typeBase
    if (!typeBase() && !consume(VOID)) {
        crtTk = startTk;
        return false;  // not a function declaration
    }

    // ✅ Function name
    if (!consume(ID)) {
        crtTk = startTk;
        return false;
    }

    // ✅ Left parenthesis
    if (!consume(LPAR)) {
        parseError("Expected '(' after function name");
    }

    // ✅ Parameter list (optional)
    if (!consume(RPAR)) {
        while (true) {
            if (!typeBase()) {
                parseError("Expected parameter type");
            }
            if (!consume(ID)) {
                parseError("Expected parameter name");
            }
            arrayDecl();  // optional

            if (consume(RPAR)) break;
            if (!consume(COMMA)) {
                parseError("Expected ',' or ')' in parameter list");
            }
        }
    }

    // ✅ Function body
    if (!stmCompound()) {
        parseError("Expected function body");
    }

    return true;
}


bool funcArg() {
    if (!typeBase()) return false;
    if (!consume(ID)) {
        parseError("Expected parameter name");
    }
    arrayDecl(); 
    return true;
}


// Parsing compound statements
bool stmCompound() {
    if (!consume(LACC)) return false;
    while (stm() || declVar()) {}
    if (!consume(RACC)) parseError("Expected '}' at end of compound statement");
    return true;
}

// Parse statement (stm)
bool stm() {
    if (stmCompound()) return true;

    if (consume(IF)) {
        expect(LPAR, "Expected '(' after 'if'");
        if (!expr()) {
            fprintf(stderr, "Expected expression after 'if('\n");
            exit(EXIT_FAILURE);
        }
        expect(RPAR, "Expected ')' after 'if' condition");
        if (!stm()) {
            fprintf(stderr, "Expected statement after 'if' condition\n");
            exit(EXIT_FAILURE);
        }
        if (consume(ELSE)) {
            if (!stm()) {
                fprintf(stderr, "Expected statement after 'else'\n");
                exit(EXIT_FAILURE);
            }
        }
        return true;
    }

    if (consume(WHILE)) {
        expect(LPAR, "Expected '(' after 'while'");
        if (!expr()) {
            fprintf(stderr, "Expected expression after 'while('\n");
            exit(EXIT_FAILURE);
        }
        expect(RPAR, "Expected ')' after 'while' condition");
        if (!stm()) {
            fprintf(stderr, "Expected statement after 'while' condition\n");
            exit(EXIT_FAILURE);
        }
        return true;
    }

    if (consume(FOR)) {
        expect(LPAR, "Expected '(' after 'for'");

        // expr? SEMICOLON expr? SEMICOLON expr?
        if (!consume(SEMICOLON)) {  // expr?
            if (!expr()) {
                fprintf(stderr, "Expected expression or ';' in 'for' init\n");
                exit(EXIT_FAILURE);
            }
            expect(SEMICOLON, "Expected ';' after 'for' init expression");
        }

        if (!consume(SEMICOLON)) { // expr?
            if (!expr()) {
                fprintf(stderr, "Expected expression or ';' in 'for' condition\n");
                exit(EXIT_FAILURE);
            }
            expect(SEMICOLON, "Expected ';' after 'for' condition expression");
        }

        if (!consume(RPAR)) { // expr?
            if (!expr()) {
                fprintf(stderr, "Expected expression or ')' in 'for' increment\n");
                exit(EXIT_FAILURE);
            }
            expect(RPAR, "Expected ')' after 'for' increment expression");
        }

        if (!stm()) {
            fprintf(stderr, "Expected statement after 'for' clause\n");
            exit(EXIT_FAILURE);
        }
        return true;
    }

    if (consume(BREAK)) {
        expect(SEMICOLON, "Expected ';' after 'break'");
        return true;
    }

    if (consume(RETURN)) {
        if (!consume(SEMICOLON)) {
            if (!expr()) {
                fprintf(stderr, "Expected expression or ';' after 'return'\n");
                exit(EXIT_FAILURE);
            }
            expect(SEMICOLON, "Expected ';' after 'return' expression");
        }
        return true;
    }

    // expr? SEMICOLON
    if (consume(SEMICOLON)) return true; // empty expr stmt

    if (expr()) {
        expect(SEMICOLON, "Expected ';' after expression statement");
        return true;
    }

    return false;
}

// expr: exprAssign

bool expr() {
    return exprAssign();
}

bool exprPrimary() {
    if (consume(ID)) {
        if (consume(LPAR)) {
            // Optional expression list
            if (!consume(RPAR)) {
                if (!expr()) {
                    parseError("Expected expression in function call");
                }
                while (consume(COMMA)) {
                    if (!expr()) {
                        parseError("Expected expression after ',' in function call");
                    }
                }
                expect(RPAR, "Expected ')' after function call arguments");
            }
        }
        return true;
    }

    if (consume(CT_INT) || consume(CT_REAL) || consume(CT_CHAR) || consume(CT_STRING)) {
        return true;
    }

    if (consume(LPAR)) {
        if (!expr()) {
            parseError("Expected expression after '('");
        }
        expect(RPAR, "Expected ')' after expression");
        return true;
    }

    return false;
}


bool exprPostfix() {
    if (!exprPrimary()) return false;

    while (true) {
        if (consume(LBRACKET)) {
            if (!expr()) {
                parseError("Expected expression inside brackets");
            }
            expect(RBRACKET, "Expected ']' after expression");
        }
        else if (consume(DOT)) {
            if (!consume(ID)) {
                parseError("Expected identifier after '.'");
            }
        }
        else {
            break;
        }
    }

    return true;
}




// exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign() {
    Token *backup = crtTk;
    if (exprUnary()) {
        if (consume(ASSIGN)) {
            if (!exprAssign()) {
                fprintf(stderr, "Expected expression after '='\n");
                exit(EXIT_FAILURE);
            }
            return true;
        } else {
            // No assignment, rollback to exprOr
            crtTk = backup;
        }
    }
    return exprOr();
}

// exprUnary: (SUB | NOT) exprUnary | exprPostfix
bool exprUnary() {
    if (consume(SUB) || consume(NOTEQ)) {  // NOTEQ token used as NOT (!) here?
        if (!exprUnary()) {
            fprintf(stderr, "Expected expression after unary operator\n");
            exit(EXIT_FAILURE);
        }
        return true;
    }
    return exprPostfix();
}

// exprOr: exprOr OR exprAnd | exprAnd
bool exprOr() {
    if (!exprAnd()) return false;

    while (consume(OR)) {
        if (!exprAnd()) {
            fprintf(stderr, "Expected expression after '||'\n");
            exit(EXIT_FAILURE);
        }
    }
    return true;
}

// exprAnd: exprAnd AND exprEq | exprEq
bool exprAnd() {
    if (!exprEq()) return false;

    while (consume(AND)) {
        if (!exprEq()) {
            fprintf(stderr, "Expected expression after '&&'\n");
            exit(EXIT_FAILURE);
        }
    }
    return true;
}

// exprEq: exprEq (EQUAL | NOTEQ) exprRel | exprRel
bool exprEq() {
    if (!exprRel()) return false;

    while (true) {
        if (consume(EQUAL) || consume(NOTEQ)) {
            if (!exprRel()) {
                fprintf(stderr, "Expected expression after '==' or '!='\n");
                exit(EXIT_FAILURE);
            }
        } else {
            break;
        }
    }
    return true;
}

// exprRel: exprRel (LESS | LESSEQ | GREATER | GREATEREQ) exprAdd | exprAdd
bool exprRel() {
    if (!exprAdd()) return false;

    while (true) {
        if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ)) {
            if (!exprAdd()) {
                fprintf(stderr, "Expected expression after relational operator\n");
                exit(EXIT_FAILURE);
            }
        } else {
            break;
        }
    }
    return true;
}

// exprAdd: exprAdd (ADD | SUB) exprMul | exprMul
bool exprAdd() {
    if (!exprMul()) return false;

    while (true) {
        if (consume(ADD) || consume(SUB)) {
            if (!exprMul()) {
                fprintf(stderr, "Expected expression after '+' or '-'\n");
                exit(EXIT_FAILURE);
            }
        } else {
            break;
        }
    }
    return true;
}

// exprMul: exprMul (MUL | DIV) exprCast | exprCast
bool exprMul() {
    if (!exprCast()) return false;

    while (true) {
        if (consume(MUL) || consume(DIV)) {
            if (!exprCast()) {
                fprintf(stderr, "Expected expression after '*' or '/'\n");
                exit(EXIT_FAILURE);
            }
        } else {
            break;
        }
    }
    return true;
}

// exprCast: LPAR typeName RPAR exprCast | exprUnary
bool exprCast() {
    Token *backup = crtTk;
    if (consume(LPAR)) {
        // parse typeName (simplified as a type keyword or ID)
        if (!(consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(VOID) || consume(ID))) {
            crtTk = backup;
            return exprUnary();
        }
        if (!consume(RPAR)) {
            fprintf(stderr, "Expected ')' after type cast\n");
            exit(EXIT_FAILURE);
        }
        if (!exprCast()) {
            fprintf(stderr, "Expected expression after cast\n");
            exit(EXIT_FAILURE);
        }
        return true;
    }
    crtTk = backup;
    return exprUnary();
}

void parse() {
    crtTk = tokens;  // Start parsing from the beginning of the token list
    unit();          // Begin parsing top-level declarations
    printf("Parsing completed successfully.\n");
}
int main() {
    const char *filename = "tests/testparser.c";

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Could not open source file");
        return EXIT_FAILURE;
    }

    tokenizeFile(f);
    fclose(f);

    parse();

    return EXIT_SUCCESS;
}


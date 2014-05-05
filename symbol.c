/*-----------------------------------------------------------------------
 Declarations for the symbol table for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:      Bandicoot 
    Members:    
 -----------------------------------------------------------------------*/

#include "symbol.h"
#include "ast.h"

typedef struct{
    char* name;
    Type type;
    int stackSlotNum;
} Declaration;

typedef struct {
    char* procName;

} List;

void addDecl(char* procName, char* varName, Type varType, int stackSlotNum) {
       
}

int inDeclared(char* varName) {
    return 0;
}

Type getType(char* varName) {
    return NULL;
}

/*-----------------------------------------------------------------------
 Declarations for the symbol table for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:      Bandicoot 
    Members:    
 -----------------------------------------------------------------------*/

#include "ast.h"

void addDecl(char* procName, char* varName, Type varType, int stackSlotNum);
bool inDeclared(char* varName);
Type getType(char* varName);


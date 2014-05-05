/*-----------------------------------------------------------------------
 Declarations for the symbol table for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:      Bandicoot 
    Members:    
 -----------------------------------------------------------------------*/

#include "ast.h"

// Add new declaration
void addDecl(char* procName, char* varName, Type varType, int stackSlotNum);

// Check if declared (1 = true, 0 = false)
int inDeclared(char* varName);

// Get type of declaration (returns type)
Type getType(char* varName);


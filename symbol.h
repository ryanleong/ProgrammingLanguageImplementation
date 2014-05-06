/*-----------------------------------------------------------------------
 Declarations for the symbol table for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:      Bandicoot 
    Members:    
 -----------------------------------------------------------------------*/

#include "ast.h"

// Add Proc
// Return 1 if success, 0 if failed
int addProc(char* procName);

// Add new declaration
// Return 1 if success, 0 if failed
int addDecl(char* procName, char* varName, Type varType, int stackSlotNum);

// Check if declared (1 = true, 0 = false)
int inDeclared(char* procName, char* varName);

// Get type of declaration (returns type)
// 1 if same type
// 0 if not same type
// -1 if declaration does not exist
// -2 if proc does not exist
int checkType(char* procName, char* varName, Type exprType);

// IGNORE THIS
void test();

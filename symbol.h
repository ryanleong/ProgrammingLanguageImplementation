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
int addDecl(char* procName, char* varName, Type varType, int stackSlotNum, int isRef, int paramNum);

// Add new array
// Return 1 if success, 0 if failed
int addArray(char* procName, char* arrayName, Type type, int stackSlotNum, int arraySize);

// Check if declared (1 = true, 0 = false)
int inDeclared(char* procName, char* varName);

// Get type of declaration (returns type)
// 1 if same type
// 0 if not same type
// -1 if declaration does not exist
// -2 if proc does not exist
int checkType(char* procName, char* varName, Type exprType);

// Get size of stack for initializing of proc
// Returns -1 if error (e.g. No such proc)
// Returns stack size (int)
int getStackSize(char* procName);

// Get stack slot number for variable
// Returns -1 if error (e.g. No such proc)
// Returns stack slot number (int)
int getStackSlotNum(char* procName, char* varName);

// Returns type of var
// Returns -1 if var is not found
Type getType(char* procName, char* varName);

// Returns 1 if var is a reference
// Returns 0 if var is a val
// Returns -1 if var does not exist
// Returns -2 if proc does not exist
int isRef(char* procName, char* varName);

// Returns 1 if param is a reference
// Returns 0 if param is a val
// Returns -1 if param does not exist
// Returns -2 if proc does not exist
// USED FOR CALLING PROCS
int isParamRef(char* procName, int paramNum);

// IGNORE THIS
void test();

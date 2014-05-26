/*-----------------------------------------------------------------------
 Declarations for the symbol table for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:       Bandicoot 
    Members:    Wee Lee Tan     377990
                Rui Cheng       664000
                Ryan Leong      395463
 -----------------------------------------------------------------------*/
#include "ast.h"

// Add Proc
// Return 1 if success, 0 if failed
int addProc(char* procName);

// Check if proc exist
int procExist(char* procName);

// Add new declaration
// Return 1 if success, 0 if failed
int addDecl(char* procName, char* varName, Type varType, int stackSlotNum, 
	int isRef, int paramNum);

// Add new array
// Return 1 if success, 0 if failed
int addArray(char* procName, char* arrayName, Type type, int stackSlotNum, 
	int arraySize, int dimension, Intervals* intervals);

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

// Returns type of param
// Returns -1 if param is not found
Type getParamType(char* procName, int paramNum);

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

// Get number of dimensions for array
// Returns -1 if error (e.g. No such proc)
int getArrayDimension(char* procName, char* name);

// Get total size of array
// Returns -1 if error (e.g. No such proc)
int getArraySize(char* procName, char* name);

// Get Intervals of array
Intervals* getIntervals(char* procName, char* name);

// IGNORE THIS
void test();

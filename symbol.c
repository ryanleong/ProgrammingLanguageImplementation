/*-----------------------------------------------------------------------
 A symbol table for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:       Bandicoot 
    Members:    Wee Lee Tan     377990
                Rui Cheng       664000
                Ryan Leong      395463
 -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"

// Declaration struct to hold variables
struct declaration {
    char* name;
    Type type;
    int stackSlotNum;
    int isRef;
    int paramNum;
    int arraySize;
    int arrayDimension;
    Intervals *intervals;
    struct declaration *next;
};

// List to hold proc struct
struct proclist {
    char* procName;
    struct declaration *firstDecl;
    struct proclist *nextProc;
};

typedef struct proclist ProcList;
typedef struct declaration Declaration;

// Proc list and Declaration list
ProcList *ListofProcs = NULL;
ProcList *LastProc = NULL;

ProcList* findProc(char* procName) {
    // Set pointer to current proc
    ProcList *currentProc = ListofProcs;

    while(currentProc) {
        // If proc is found
        if(strcmp(currentProc->procName, procName) == 0) {
            // return pointer
            return currentProc;
        } 

        // iterate to next proc
        currentProc = currentProc->nextProc;
    }

    return NULL;
}

int isParamRef(char* procName, int paramNum) {
    // If proc list is not empty
    if (ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if (proc) {

            Declaration *r = proc->firstDecl;

            while(r) {

                // if parameter exist
                if (r->paramNum == paramNum) {

                    if(r->paramNum > -1 && r->isRef == 1) {
                        // return true
                        return 1;
                    }
                    else {
                        // return false
                        return 0;
                    }
                }

                r = r->next;
            }

            // If parameter does not exist
            return -1;

        } else {
            // Proc does not exist
            return -2;
        }
    }

    // Proc list empty
    return -2;
}

int isRef(char* procName, char* varName) {
    // If proc list is not empty
    if (ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if (proc) {

            Declaration *r = proc->firstDecl;

            while(r) {
                // if reference exist
                if (strcmp(r->name, varName) == 0) {
                    return r->isRef;
                }

                r = r->next;
            }

            // If reference does not exist
            return -1;

        } else {
            // Proc does not exist
            return -2;
        }
    }

    // Proc list empty
    return -2;
}


int addProc(char* procName) {

    // Create new proc
    ProcList *temp = (ProcList *) malloc(sizeof(ProcList));
    temp->procName = procName;
    temp->firstDecl = NULL;
    temp->nextProc = NULL;

    if(ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if(proc) {
            free(temp);
            return 0;
        }
        else {
            // place new proc at end of list
            LastProc->nextProc = temp;
            LastProc = temp;
        }
    }
    else {
        // place proc in list if list is empty
        ListofProcs = temp;
        LastProc = temp;
    }

    return 1;
}

int procExist(char* procName) {

    // check if proc exist
    if(findProc(procName) != NULL) {
        return 1;
    }
    else {
        return 0;
    }
}

int addDecl(char* procName, char* varName, Type varType, int stackSlotNum, 
    int isRef, int paramNum) {

    if(ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if(proc) {
            // add declaration
            Declaration *d = (Declaration *) malloc(sizeof(Declaration));
            d->name = varName;
            d->type = varType;
            d->isRef = isRef;
            d->paramNum = paramNum;
            d->stackSlotNum = stackSlotNum;

            // place declaration at start of list
            if (proc->firstDecl == NULL) {
                proc->firstDecl = d;
                return 1;
            }
            else {
                Declaration *dec = proc->firstDecl;

                int isDeclared = 0;
                int paramNumUsed = 0;

                // Insert declaration at end of list
                while(dec) {

                    // Check if exist
                    if(strcmp(dec->name, varName) == 0) {
                        isDeclared = 1;
                    }

                    // Check if param number is used
                    if (dec->paramNum != -1 && dec->paramNum == paramNum) {
                        paramNumUsed = 1;
                    }

                    if (dec->next == NULL && isDeclared == 0 
                        && paramNumUsed == 0) {
                        dec->next = d;
                        return 1;
                    }

                    dec = dec->next;
                }
            }

            // Free memory if not stored
            free(d);
        }

    }


    // Failed when no procs exist
    return 0;
}


int addArray(char* procName, char* arrayName, Type type, int stackSlotNum, 
    int arraySize, int dimension, Intervals *intervals) {

    if(ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if(proc) {
            // add declaration
            Declaration *d = (Declaration *) malloc(sizeof(Declaration));
            d->name = arrayName;
            d->type = type;
            d->arraySize = arraySize;
            d->arrayDimension = dimension;
            d->intervals = intervals;
            d->stackSlotNum = stackSlotNum;

            // place declaration at start of list
            if (proc->firstDecl == NULL) {
                proc->firstDecl = d;
                return 1;
            }
            else {
                Declaration *dec = proc->firstDecl;

                int isDeclared = 0;

                // Insert declaration at end of list
                while(dec) {

                    // Check if exist
                    if(strcmp(dec->name, arrayName) == 0) {
                        isDeclared = 1;
                    }

                    if (dec->next == NULL && isDeclared == 0) {
                        dec->next = d;
                        return 1;
                    }

                    dec = dec->next;
                }
            }

            // Free memory if not stored
            free(d);
        }

    }


    // Failed when no procs exist
    return 0;
}

Intervals* getIntervals(char* procName, char* name) {
    // If proc list not empty
    if (ListofProcs) {
        // Find proc
        ProcList *proc = findProc(procName);
        
        // Check if proc exist
        if (proc) {
            // Get first declaration
            Declaration *d = proc->firstDecl;
            
            while (d) {
                
                // if declaration exist
                if (strcmp(d->name, name) == 0) {

                    // Check if var is array
                    if (d->type == INT_ARRAY_TYPE || 
                        d->type == FLOAT_ARRAY_TYPE || 
                        d->type == BOOL_ARRAY_TYPE) {

                        // return stack slot number
                        return d->intervals;    
                    }
                    else {
                        return NULL;
                    }
                }

                d = d->next;
            }
            
        }
    }
    
    // Proc list empty
    return NULL;
}

int inDeclared(char* procName, char* varName) {

    // If proc list is not empty
    if (ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if (proc) {

            Declaration *d = proc->firstDecl;

            while(d) {

                // if declaration exist
                if (strcmp(d->name, varName) == 0) {
                    return 1;
                }

                d = d->next;
            }

        } else {
            // Proc does not exist
            return 0;
        }
    }

    // Proc list empty
    return 0;
}

Type getType(char* procName, char* varName){
    
    // If proc list is not empty
    if (ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if (proc) {

            Declaration *d = proc->firstDecl;

            while(d) {

                // if declaration exist
                if (strcmp(d->name, varName) == 0) {
                    return d->type;
                }

                d = d->next;
            }

            // If declaration does not exist
            return -1;

        } else {
            // Proc does not exist
            return -2;
        }
    }

    // Proc list empty
    return -2;
}

Type getParamType(char* procName, int paramNum) {
    // If proc list is not empty
    if (ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if (proc) {

            Declaration *d = proc->firstDecl;

            while(d) {

                // if declaration exist
                if (d->paramNum == paramNum) {
                    return d->type;
                }

                d = d->next;
            }

            // If declaration does not exist
            return -1;

        } else {
            // Proc does not exist
            return -2;
        }
    }

    // Proc list empty
    return -2;
}

int checkType(char* procName, char* varName, Type exprType) {

    // If proc list is not empty
    if (ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if (proc) {

            Declaration *d = proc->firstDecl;

            while(d) {
                // if declaration exist
                if (strcmp(d->name, varName) == 0) {

                    // check if same type
                    if (d->type == exprType) {
                        
                        // Same Type
                        return 1;
                    }
                    else {
                        // Different Type
                        return 0;
                    }
                }

                d = d->next;
            }

            // If declaration does not exist
            return -1;

        } else {
            // Proc does not exist
            return -2;
        }
    }

    // Proc list empty
    return -2;
}

int getArrayDimension(char* procName, char* name){
    // If proc list not empty
    if (ListofProcs) {
        // Find proc
        ProcList *proc = findProc(procName);
        
        // Check if proc exist
        if (proc) {
            // Get first declaration
            Declaration *d = proc->firstDecl;
            
            while (d) {
                
                // if declaration exist
                if (strcmp(d->name, name) == 0) {
                    // Check if var is array
                    if (d->type == INT_ARRAY_TYPE || 
                        d->type == FLOAT_ARRAY_TYPE || 
                        d->type == BOOL_ARRAY_TYPE) {

                        // return stack slot number
                        return d->arrayDimension;   
                    }
                    else {
                        return -1;
                    }
                }

                d = d->next;
            }
            
        }
    }
    
    // Proc list empty
    return -1;
}

int getArraySize(char* procName, char* name) {
    // If proc list not empty
    if (ListofProcs) {
        // Find proc
        ProcList *proc = findProc(procName);
        
        // Check if proc exist
        if (proc) {
            // Get first declaration
            Declaration *d = proc->firstDecl;
            
            while (d) {
                
                // if declaration exist
                if (strcmp(d->name, name) == 0) {

                    // Check if var is array
                    if (d->type == INT_ARRAY_TYPE || 
                        d->type == FLOAT_ARRAY_TYPE || 
                        d->type == BOOL_ARRAY_TYPE) {

                        // return stack slot number
                        return d->arraySize;    
                        
                    }
                    else {
                        return -1;
                    }
                    
                }

                d = d->next;
            }
            
        }
    }
    
    // Proc list empty
    return -1;
}

int getStackSize(char* procName) {
    
    // If proc list not empty
    if (ListofProcs) {
        // Find proc
        ProcList *proc = findProc(procName);
        
        // Check if proc exist
        if (proc) {
            // Get first declaration
            Declaration *d = proc->firstDecl;
            
            int total = 0; 
            
            while (d) {

                if(d->next == NULL) {
                    // if not array
                    if (d->type == INT_ARRAY_TYPE || 
                        d->type == FLOAT_ARRAY_TYPE || 
                        d->type == BOOL_ARRAY_TYPE) {
                        
                        return (d->stackSlotNum + d->arraySize);
                    }
                    else {
                        return d->stackSlotNum+1;
                    }
                }

                d = d->next;
            }
            
            return total;
        }
    }
    
    // Proc list empty
    return -1;
}

int getStackSlotNum(char* procName, char* varName) {
    
    // If proc list not empty
    if (ListofProcs) {
        // Find proc
        ProcList *proc = findProc(procName);
        
        // Check if proc exist
        if (proc) {
            // Get first declaration
            Declaration *d = proc->firstDecl;
            
            while (d) {
                
                // if declaration exist
                if (strcmp(d->name, varName) == 0) {
                    // return stack slot number
                    return d->stackSlotNum;
                }

                d = d->next;
            }
            
        }
    }
    
    // Proc list empty
    return -1;
}

/*-----------------------------------------------------------------------
 Declarations for the symbol table for the Wiz programs.
 For use in the COMP90045 project 2014.

    Team:      Bandicoot 
    Members:    
 -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"

struct declaration {
    char* name;
    Type type;
    int stackSlotNum;
    struct declaration *next;
};

struct proclist {
    char* procName;
    struct declaration *firstDecl; 
    struct proclist *nextProc;
};

typedef struct proclist ProcList;
typedef struct declaration Declaration;

ProcList *ListofProcs = NULL;
ProcList *LastProc = NULL;

ProcList* findProc(char* procName);

void test() {
    printf("\n\n");
    
//    addDecl("getName", "sdf", 2, 2);
//    addDecl("main", "stuff", 1, 3);
//    addDecl("main", "things", STRING_TYPE, 3);
//    addDecl("getName", "9393", 2, 2);
//
//    printf("%d\n", getType("getName", "93933"));

    addProc("main");
    addProc("test");
    addProc("dsfdsf");

    addDecl("main", "temp", INT_TYPE, 0);
    addDecl("main", "temp", STRING_TYPE, 0);
    addDecl("main", "tempd", INT_TYPE, 1);
    addDecl("test", "d", BOOL_TYPE, 0);
    

    printf("Type compare: %d\n", checkType("test", "dd", BOOL_TYPE));
    printf("Stack Size of main: %d\n", getStackSize("main"));
    printf("Stack Slot No.: %d", getStackSlotNum("main", "tempd"));

    //////////////////////////////////////////////////////
    // Print out everything
    
    printf("\n");
    
    while(ListofProcs) {
        printf("ProcName: %s", ListofProcs->procName);

        Declaration *d = ListofProcs->firstDecl;

        while(d) {
            printf(", VarName: %s, Type: %d", d->name, d->type);
            d = d->next;
        }
        
        printf("\n");

        ListofProcs = ListofProcs->nextProc;
    }
    
    printf("\n\n");
}

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

int addProc(char* procName) {
    if(ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if(proc) {
        	return 0;
        }
        else {
            // create new proc at end of list
            ProcList *temp = (ProcList *) malloc(sizeof(ProcList));
            temp->procName = procName;
            temp->firstDecl = NULL;
            temp->nextProc = NULL;

            LastProc->nextProc = temp;
            LastProc = temp;
        }
    }
    else {
        // When proc list is empty
        // create new proc
        ProcList *temp = (ProcList *) malloc(sizeof(ProcList));
        temp->procName = procName;
        temp->firstDecl = NULL;
        temp->nextProc = NULL;

        ListofProcs = temp;
        LastProc = temp;
    }

    return 1;
}

int addDecl(char* procName, char* varName, Type varType, int stackSlotNum) {

    if(ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if(proc) {
            // add declaration
            Declaration *d = (Declaration *) malloc(sizeof(Declaration));
            d->name = varName;
            d->type = varType;
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
                	if(strcmp(dec->name, varName) == 0) {
                		isDeclared = 1;
                	}

                    if (dec->next == NULL && isDeclared == 0) {
                        dec->next = d;
                        return 1;
                    }

                    dec = dec->next;
                }
            }
        }

    }

    // Failed when no procs exist
	return 0;
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
				total++;
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

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
    
    addDecl("getName", "sdf", 2, 2);
    addDecl("main", "stuff", 1, 3);
    addDecl("main", "things", 1, 3);
    addDecl("getName", "9393", 2, 2);

    printf("%d\n", inDeclared("f", "things"));

    //////////////////////////////////////////////////////
    // Print out everything
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

void addDecl(char* procName, char* varName, Type varType, int stackSlotNum) {

    // add declaration
    Declaration *d = (Declaration *) malloc(sizeof(Declaration));
    d->name = varName;
    d->type = varType;
    d->stackSlotNum = stackSlotNum;

    if(ListofProcs) {
        // check if proc exist
        ProcList *proc = findProc(procName);

        if(proc) {

        	// place declaration at start of list
            if (proc->firstDecl == NULL) {
            	proc->firstDecl = d;
			}
            else {
            	Declaration *dec = proc->firstDecl;

            	// Insert declaration at end of list
                while(dec) {
                    if (dec->next == NULL) {
                        dec->next = d;
                        break;
                    }

                    dec = dec->next;
                }
            }
        }
        else {
            // create new proc at end of list
            ProcList *temp = (ProcList *) malloc(sizeof(ProcList));
            temp->procName = procName;
            temp->firstDecl = d;
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
        temp->firstDecl = d;
        temp->nextProc = NULL;

        ListofProcs = temp;
        LastProc = temp;
    }
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

Type getType(char* procName, char* varName) {
    return 0;
}

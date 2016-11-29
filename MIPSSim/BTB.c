
/************************************************************************/
/*BTB.c                                                                 */
/*Authorized by: Qiao Chu                                               */
/************************************************************************/

#include "btb.h"
#include "MIPSSim.h"
#include "Prototype.h"

/* Find Least Recently used Branch*/

BTBEntry* findLRUEntry(MipsSimContext *pContext)
{
    UINT        min     = 1000;
    BTBEntry    *pointer   = pContext->BTB;
    BTBEntry    *lru    = NULL;
    
    while (pointer != NULL)
    {
        if (pointer->HitRate < min)
        {
            min = pointer->HitRate;
            lru = pointer;
        }
        pointer = pointer->next;
    }
    
    return lru;
}

//Checks if instruction is present in BTB, if it is really in BTB return the pointer of that particular branch instruction

BTBEntry* checkInstructionInBTB(MipsSimContext *pContext, UINT InstAddress)
{
    BTBEntry *pointer = pContext->BTB;

    while (pointer != NULL)
    {
        if (pointer->InstAddress == InstAddress)
        {
            break;
        }
        pointer = pointer->next;
    }

    return pointer;
}

/*Get size of the pointerent BTB */

UINT getSizeofBTB(MipsSimContext *pContext)
{
    UINT        count   = 0;
    BTBEntry    *pointer   = pContext->BTB;

    while (pointer != NULL)
    {
        pointer = pointer->next;
        count++;
    }

    return count;
}




// Insert a new branch entry in to BTB under following conditions
//if current size of BTB is greater than 16 then it replaces the least recently used BTB entry
// else a new record is added to BTB



void insertNewPredAtISStage(MipsSimContext *pContext, BTBEntry *newEntry)
{
    BTBEntry    *leastUsed  = NULL;
    BTBEntry    *pointer       = NULL;

    if (pContext->BTB == NULL)
    {
        pContext->BTB = newEntry;
    }
    else
    {
        if (getSizeofBTB(pContext) >= BTB_MAX_SIZE)
        {
            /* Implement LRU policy in the BTB*/
            leastUsed = findLRUEntry(pContext);
            leastUsed->HitRate = 1;
            leastUsed->InstAddress = newEntry->InstAddress;
            leastUsed->TargetAddress = newEntry->TargetAddress;
            leastUsed->PredState = newEntry->PredState;
            free(newEntry);
        }
        else
        {
            pointer = pContext->BTB;
            while (pointer->next != NULL)
            {
                pointer = pointer->next;
            }
            pointer->next = newEntry;
        }
    }
}

// Fetch the next instruction distance from the current instruction which is
// at relative distance of 4 or at relative distance of (target Instruction Address- current
// Instruction Address)

UINT getNextPCAddress(MipsSimContext *pContext, UINT InstAddress)
{
    BTBEntry    *branchDetails  = checkInstructionInBTB(pContext, InstAddress);
    UINT        NextPCAddress   = InstAddress;
    Instruction *pInst = getInstFromList(pContext, InstAddress);
    BTBEntry    *pBTBEntry = NULL;

    if (branchDetails != NULL)
    {
        branchDetails->HitRate++;

        if (branchDetails->PredState == BRANCH_TAKEN)
        {
            
            
            NextPCAddress = branchDetails->TargetAddress;
            
            pInst->BranchPred = BRANCH_TAKEN;
        }
        else
        {
           
            NextPCAddress += 4;
            pInst->BranchPred = BRANCH_NOT_TAKEN;
        }
    }
    
    else
    {
        NextPCAddress += 4;
    }

    return NextPCAddress;
}

void updatePredAtEXStage(MipsSimContext *pContext, UINT InstAddress, UINT PredState)
{
    BTBEntry    *pBTBEntry = NULL;
    pBTBEntry = checkInstructionInBTB(pContext, InstAddress);
    pBTBEntry->PredState = PredState;
}


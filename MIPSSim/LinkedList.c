/************************************************************************/
/*LinkedList.c                                                          */
/*Authorized by: Qiao Chu                                               */
/************************************************************************/
#include "Prototype.h"

void addInstToList(MipsSimContext *pContext, Instruction  *pInst)
{
    Instruction     *pLocalInst = NULL;
    Instruction     *pCurrInst = NULL;

    
    pLocalInst = (Instruction*)malloc(sizeof(Instruction));
    memcpy(pLocalInst, pInst, sizeof(Instruction));

    
    if (pContext->InstructionList == NULL)
    {
        
        pContext->InstructionList = pLocalInst;
    }
    else
    {
        
        pCurrInst = pContext->InstructionList;
        while (pCurrInst->next != NULL)
        {
            pCurrInst = pCurrInst->next;
        }
        pCurrInst->next = pLocalInst;
    }
}


void addDataToList(MipsSimContext *pContext, INT ReadDWord)
{
    Data    *pLocalData = NULL;
    Data    *pCurrData = NULL;

   
    pLocalData = (Data*)malloc(sizeof(Data));
    pLocalData->DataAddress = pContext->CurrentAddress;
    pLocalData->DataDWord = ReadDWord;
    pLocalData->next = NULL;

  
    if (pContext->DataList == NULL)
    {
        
        pContext->DataList = pLocalData;
    }
    else
    {
        
        pCurrData = pContext->DataList;
        while (pCurrData->next != NULL)
        {
            pCurrData = pCurrData->next;
        }
        pCurrData->next = pLocalData;
    }
}


Instruction* getInstFromList(MipsSimContext *pContext, UINT InstAddress)
{
    Instruction     *pCurrInst = NULL;
    Instruction     *pTempInst = NULL;

    if (pContext->InstructionList != NULL)
    {
        pCurrInst = pContext->InstructionList;

        if (pCurrInst->InstAddress == InstAddress)
        {
            pTempInst = pCurrInst;
        }
        else
        {
            while (pCurrInst->next != NULL)
            {
                pCurrInst = pCurrInst->next;
                if (pCurrInst->InstAddress == InstAddress)
                {
                    pTempInst = pCurrInst;
                    break;
                }
            }
        }
    }

    return pTempInst;
}


Data* getDataFromList(MipsSimContext *pContext, UINT Addr)
{
    Data     *pCurrData = NULL;
    Data     *pTempData = NULL;

    if (pContext->DataList != NULL)
    {
        pCurrData = pContext->DataList;

        if (pCurrData->DataAddress == Addr)
        {
            pTempData = pCurrData;
        }
        else
        {
            while (pCurrData->next != NULL)
            {
                pCurrData = pCurrData->next;
                if (pCurrData->DataAddress == Addr)
                {
                    pTempData = pCurrData;
                }
            }
        }
    }

    return pTempData;
}

void setDataToList(MipsSimContext *pContext, Data *pData)
{
    Data     *pCurrData = NULL;

    if (pContext->DataList != NULL)
    {
        pCurrData = pContext->DataList;

        if (pCurrData->DataAddress == pData->DataAddress)
        {
            pCurrData->DataDWord = pData->DataDWord;
        }
        else
        {
            while (pCurrData->next != NULL)
            {
                pCurrData = pCurrData->next;
                if (pCurrData->DataAddress == pData->DataAddress)
                {
                    pCurrData->DataDWord = pData->DataDWord;
                    break;
                }
            }
        }
    }
}

void addInstToInstQueue(MipsSimContext *pContext, Instruction *pInst)
{
    Instruction *pCurrInst = NULL;
    Instruction *pTempInst = NULL;

    pTempInst = (Instruction*)malloc(sizeof(Instruction));
    memcpy(pTempInst, pInst, sizeof(Instruction));

    pTempInst->PipelineStage = INSTRUCTION_FETCH;
    pTempInst->next = NULL;

    if (pContext->InstQueue == NULL)
    {
        pContext->InstQueue = pTempInst;
    }
    else
    {
        pCurrInst = pContext->InstQueue;
        while (pCurrInst->next != NULL)
        {
            pCurrInst = pCurrInst->next;
        }
        pCurrInst->next = pTempInst;
    }
}


void moveInstToISStage(MipsSimContext *pContext)
{
    Instruction     *pCurrInst = pContext->InstQueue;


    if (pCurrInst == NULL)
    {
    }
    else
    {
        while (pCurrInst->next != NULL)
        {
            pCurrInst = pCurrInst->next;
        }
        

        if (pCurrInst->PipelineStage == INSTRUCTION_FETCH)
        {
            pCurrInst->PipelineStage = INSTRUCTION_ISSUE;
        }
    }
}


bool IsInstQueueEmpty(MipsSimContext *pContext)
{
    return (pContext->InstQueue == NULL) ? true : false;
}


Instruction* getInstToIssue(MipsSimContext *pContext)
{
    Instruction*    pCurrInst = pContext->InstQueue;
    Instruction*    pTempInst = NULL;

    if (pCurrInst != NULL)
    {
        if (pCurrInst->PipelineStage == INSTRUCTION_ISSUE)
        {
            pTempInst = pCurrInst;
        }
    }

    return pTempInst;
}


void removeInstFromInstQueue(MipsSimContext *pContext, Instruction *pInst)
{
    Instruction*    pCurrInst = pContext->InstQueue;
    Instruction*    pPrevInst = pContext->InstQueue;


    if (pCurrInst == pInst)
    {
        pContext->InstQueue = pCurrInst->next;
        pCurrInst->next = NULL;
    }
    else
    {

        while (pCurrInst->next != NULL)
        {
            pPrevInst = pCurrInst;
            pCurrInst = pCurrInst->next;

            if (pCurrInst == pInst)
            {
                pPrevInst->next = pCurrInst->next;
                pCurrInst->next = NULL;
                break;
            }
        }
    }
}


UINT reorderROBInst(MipsSimContext *pContext, Instruction *ROBInst)
{
    UINT    ROBCount = 0;
    UINT    ROBInstCount = 0;
    UINT    i = 0, j = 0;
    UINT    minInstIssueNum = 0;
    Instruction tempInst;

    for (ROBCount = 0; ROBCount < MAX_ROB_ENTRIES; ROBCount++)
    {
        if (pContext->ROB[ROBCount].Busy)
        {
            memcpy(&ROBInst[ROBInstCount++], pContext->ROB[ROBCount].pInst, sizeof(Instruction));
        }
    }

    for (i = 1; i < ROBInstCount; i++)
    {
        j = i;
        while (j > 0)
        {
            if (ROBInst[j].InstIssueNum < ROBInst[j - 1].InstIssueNum)
            {
                memcpy(&tempInst, &ROBInst[j - 1], sizeof(Instruction));
                memcpy(&ROBInst[j - 1], &ROBInst[j], sizeof(Instruction));
                memcpy(&ROBInst[j], &tempInst, sizeof(Instruction));
            }
            else break;
            j--;
        }
    }

    return ROBInstCount;
}


UINT reorderRSInst(MipsSimContext *pContext, Instruction *RSInst)
{
    UINT    RSCount = 0;
    UINT    RSInstCount = 0;
    UINT    i = 0, j = 0;
    UINT    minInstIssueNum = 0;
    Instruction tempInst;

    for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
    {
        if (pContext->RS[RSCount].Busy)
        {
            memcpy(&RSInst[RSInstCount++], pContext->RS[RSCount].pInst, sizeof(Instruction));
        }
    }

    for (i = 1; i < RSInstCount; i++)
    {
        j = i;
        while (j > 0)
        {
            if (RSInst[j].InstIssueNum < RSInst[j - 1].InstIssueNum)
            {
                memcpy(&tempInst, &RSInst[j - 1], sizeof(Instruction));
                memcpy(&RSInst[j - 1], &RSInst[j], sizeof(Instruction));
                memcpy(&RSInst[j], &tempInst, sizeof(Instruction));
            }
            else break;
            j--;
        }
    }

    return RSInstCount;
}


UINT getROBEntryOldestInst(MipsSimContext *pContext)
{
    Instruction     ROBInst[MAX_RS_ENTRIES];
    UINT            ROBInstCount = 0;
    UINT            count = 0;

    ROBInstCount = reorderROBInst(pContext, ROBInst);


    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (pContext->ROB[count].Busy && pContext->ROB[count].pInst->InstIssueNum == ROBInst[0].InstIssueNum)
        {
            break;
        }
    }

    return count;
}


bool IsROBEmpty(MipsSimContext *pContext)
{
    UINT count = 0;

    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (pContext->ROB[count].Busy)
        {
            return false;
        }
    }

    return true;
}


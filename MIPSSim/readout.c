
/************************************************************************/
/*readout.c                                                             */
/*Authorized by: Qiao Chu                                               */
/************************************************************************/

#include "Prototype.h"


bool verifyInOutFiles(MipsSimContext *pContext)
{
    bool    ReturnStatus = true;

    do
    {
        
        pContext->FileHandle.InputFileHandle = fopen(pContext->Args.InputFilename, "r");
        //pContext->FileHandle.InputFileHandle = fopen("/Users/qiaochu/Desktop/MIPSSim/fibonacci_bin.bin", "r");
        
        if (pContext->FileHandle.InputFileHandle == NULL)
        {
            printf("Unable to open Input file\n");
            ReturnStatus = false;
            break;
        }

     
        fseek(pContext->FileHandle.InputFileHandle, 0, SEEK_END);
        pContext->InputFileLength = (UINT)ftell(pContext->FileHandle.InputFileHandle);
        fseek(pContext->FileHandle.InputFileHandle, 0, SEEK_SET);

    
        char *p = pContext->Args.OutputFilename;
        int m = strlen(p);
      
        p[m-2] = 't';
        p[m-1] = '\0';
        //pContext->FileHandle.OutputFileHandle = fopen(pContext->Args.OutputFilename, "wt");
        pContext->FileHandle.OutputFileHandle = fopen(p, "wt");
        
        //pContext->FileHandle.OutputFileHandle = fopen("/Users/qiaochu/Desktop/MIPSSim/output.txt", "wt");
        
        if (pContext->FileHandle.OutputFileHandle == NULL)
        {
            printf("Cannot write to the Output file!\n");
            ReturnStatus = false;
            break;
        }

    } while (false);

    if (!ReturnStatus)
    {
        closeFiles(pContext);
    }

    return ReturnStatus;
}


UINT readUnsignedDWordFile(FILE *fp)
{
    union _DWord ReadDWord;

    if (fread(&ReadDWord.Byte, sizeof(char), 4, fp))
    {
        
        swapBytes(&ReadDWord.Byte[0], &ReadDWord.Byte[3]);
        swapBytes(&ReadDWord.Byte[1], &ReadDWord.Byte[2]);
    }

    return ReadDWord.UnsignedDwordValue;
}


INT readSignedDWordFile(FILE *fp)
{
    union _DWord ReadDWord;

    if (fread(&ReadDWord.Byte, sizeof(char), 4, fp))
    {
       
        swapBytes(&ReadDWord.Byte[0], &ReadDWord.Byte[3]);
        swapBytes(&ReadDWord.Byte[1], &ReadDWord.Byte[2]);
    }

    return ReadDWord.SignedDWordValue;
}



void closeFiles(MipsSimContext *pContext)
{
    if (pContext->FileHandle.InputFileHandle)
    {
        fclose(pContext->FileHandle.InputFileHandle);
    }

    if (pContext->FileHandle.OutputFileHandle)
    {
        fclose(pContext->FileHandle.OutputFileHandle);
    }
}

void printOutputFile(MipsSimContext *pContext)
{
    Instruction     *pInst = NULL;
    UINT            count = 0;
    Data            *pData = NULL;
    UINT            DataCount = 0;
    Instruction     ROBInst[MAX_ROB_ENTRIES];
    Instruction     RSInst[MAX_RS_ENTRIES];
    UINT            ROBInstCount = 0, RSInstCount = 0;
    BTBEntry        *currBTBEntry = pContext->BTB;

    
    ROBInstCount = reorderROBInst(pContext, ROBInst);
    RSInstCount = reorderRSInst(pContext, RSInst);

    memset(pContext->Buffer, 0, sizeof(pContext->Buffer));
    pContext->currentBufferPos = 0;

    if (pContext->Args.PrintFinalState || pContext->Args.PrintAllCycles ||
        (pContext->CurrentCycle >= (UINT)pContext->Args.StartCycleNum && pContext->CurrentCycle <= (UINT)pContext->Args.EndCycleNum))
    {
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Cycle <%d>:\n", pContext->CurrentCycle);

        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "IQ:\n");
        pInst = pContext->InstQueue;
        while (pInst)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%s", pInst->InstString);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]\n");
            pInst = pInst->next;
        }

        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "RS:\n");
        for (count = 0; count < RSInstCount; count++)
        {
            pInst = &RSInst[count];
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%s", pInst->InstString);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]\n");
        }

        // Add Instructions in the ROB
        // Instructions in the ROB should be printed in the order from oldest to newest 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "ROB:\n");
        for (count = 0; count < ROBInstCount; count++)
        {
            pInst = &ROBInst[count];
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%s", pInst->InstString);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]\n");
        }

        // Add BTB Data 
        count = 1;
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "BTB:\n");
        while (currBTBEntry != NULL)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "[");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Entry %d", count++);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "]:<");
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d,", currBTBEntry->InstAddress);
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d,", currBTBEntry->TargetAddress);
            if (currBTBEntry->PredState == NOT_SET)
            {
                pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "NotSet");
            }
            else
            {
                if (currBTBEntry->PredState == BRANCH_TAKEN)
                {
                    pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "1");
                }
                else
                {
                    pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "0");
                }
            }

            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], ">\n");

            currBTBEntry = currBTBEntry->next;
        }

        // Add Register Values 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Registers:\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R00:    ");
        for (count = 0; count < 7; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[7].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R08:    ");
        for (count = 8; count < 15; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[15].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R16:    ");
        for (count = 16; count < 23; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[23].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "R24:    ");
        for (count = 24; count < 31; count++)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", RegLookUpTable[count].RegValue);
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", RegLookUpTable[31].RegValue);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");

        // Add Data Segement values 
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "Data Segment:\n");
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "716:    ");
        pData = pContext->DataList;
        while (pData)
        {
            pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%-8d", pData->DataDWord);
            DataCount++;

            if (DataCount == 9)
            {
                break;
            }
            pData = pData->next;
        }
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "%d", pData->next->DataDWord);
        pContext->currentBufferPos += sprintf(&pContext->Buffer[pContext->currentBufferPos], "\n");

        // Write the buffer to the file 
        fwrite(pContext->Buffer, pContext->currentBufferPos, 1, pContext->FileHandle.OutputFileHandle);
    }
}


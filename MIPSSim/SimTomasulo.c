
/************************************************************************/
/*SimTomasulo.c                                                         */
/*Authorized by: Qiao Chu                                               */
/************************************************************************/

#include "Prototype.h"
int a = 0;
int b = 0;
int c = 0;
bool getAvailableROBEntry(MipsSimContext* pContext, UINT* ROBEntry)
{
    UINT count = 0;

    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (!pContext->ROB[count].Busy)
        {/*
            if(pContext->ROB[count].decideempty == 2){
                pContext->ROB[count].decideempty = 1;
                continue;
            }
          */
            if(c == 2){
                c = 1;
                continue;
            }
            *ROBEntry = count;
            return true;
          
        }
    }

    return false;
}

bool getAvailableRSEntry(MipsSimContext* pContext, UINT* RSEntry)
{
    UINT count = 0;

    for (count = 0; count < MAX_RS_ENTRIES; count++)
    {
        if (!pContext->RS[count].Busy)
        {
            *RSEntry = count;
            return true;
        }
    }

    return false;
}

bool IsSimComplete(MipsSimContext *pContext)
{
    UINT        count = 0;

    for (count = 0; count < MAX_ROB_ENTRIES; count++)
    {
        if (pContext->ROB[count].Busy)
        {
            return false;
        }
    }

    if (!IsInstQueueEmpty(pContext))
    {
        return false;
    }
    
    if(a == 3){
        a = 0;
        return false;
    }
    
     
    return true;
}

void fetchInst(MipsSimContext *pContext)
{
    Instruction *pInst = NULL;

    moveInstToISStage(pContext);

    pInst = getInstFromList(pContext, pContext->CurrentPC);
    if (pInst)
    {
        pInst->InstIssueNum = pContext->CurrentInstIssueNum++;

        pContext->CurrentPC = getNextPCAddress(pContext, pContext->CurrentPC);

        addInstToInstQueue(pContext, pInst);
    }
}

void issueInst(MipsSimContext* pContext)
{
    Instruction *pInst      = NULL;
    UINT        ROBEntry    = 0;
    UINT        RSEntry     = 0;
    BTBEntry    *pBTBEntry  = NULL;

    pInst = getInstToIssue(pContext);
    //test
    //if(pInst){
    //    printf("%d\n", pContext->CurrentCycle);
    //}
    //
    if (pInst && pInst->IsBreakOrNopInst && getAvailableROBEntry(pContext, &ROBEntry))
    {
        removeInstFromInstQueue(pContext, pInst);

        pContext->ROB[ROBEntry].pInst = pInst;
        pContext->ROB[ROBEntry].Ready = true;
        pContext->ROB[ROBEntry].Busy = true;
        pContext->ROB[ROBEntry].Value = 0;

        pInst->UpdatePipelineStage = true;
    }
    else
    {

        
        if(pInst){
            if(getAvailableRSEntry(pContext, &RSEntry)){
                if(getAvailableROBEntry(pContext, &ROBEntry)){
                    printf("%d\n", pContext->CurrentCycle);
 
        {

            
            removeInstFromInstQueue(pContext, pInst);
            

            if (pInst->IsRsUsedAsOperand)
            {
                if (pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Busy)
                {
                   
                    if (pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Reorder].Ready)
                    {
                        pContext->RS[RSEntry].Vj = pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Reorder].Value;
                        pContext->RS[RSEntry].Qj = MAX_ROB_ENTRIES;
                    }
                    else
                    {
                        pContext->RS[RSEntry].Qj = pContext->RegisterStat[pInst->InstructionDWord.RType.Rs].Reorder;
                    }
                }
                else
                {
                    pContext->RS[RSEntry].Vj = RegLookUpTable[pInst->InstructionDWord.RType.Rs].RegValue;
                    pContext->RS[RSEntry].Qj = MAX_ROB_ENTRIES;
                }
            }

            if (pInst->IsRtUsedAsOperand)
            {
                if (pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Busy)
                {
                   
                    if (pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder].Ready)
                    {
                        pContext->RS[RSEntry].Vk = pContext->ROB[pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder].Value;
                        pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
                    }
                    else
                    {
                        pContext->RS[RSEntry].Qk = pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder;
                    }
                }
                else
                {
                    pContext->RS[RSEntry].Vk = RegLookUpTable[pInst->InstructionDWord.RType.Rt].RegValue;
                    pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
                }
            }
            
            if (pInst->IsImmALUInst)
            {
                pContext->RS[RSEntry].Vk = pInst->InstructionDWord.IType.SignedImmediate;
                pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
            }

            if (pInst->IsShiftALUInst)
            {
                pContext->RS[RSEntry].Vk = pInst->InstructionDWord.RType.Shamt;
                pContext->RS[RSEntry].Qk = MAX_ROB_ENTRIES;
            }

            pContext->ROB[ROBEntry].pInst = pInst;
            pContext->RS[RSEntry].pInst = pInst;
            pContext->ROB[ROBEntry].Ready = false;
            pContext->ROB[ROBEntry].Busy = true;
            pContext->RS[RSEntry].Busy = true;
            pContext->RS[RSEntry].Dest = ROBEntry;
            
            pInst->UpdatePipelineStage = true;

            if (pInst->IsRdUsedAsDest)
            {
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rd].Reorder = ROBEntry;
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rd].Busy = true;
                pContext->ROB[ROBEntry].Dest = pInst->InstructionDWord.RType.Rd;
            }

            if (pInst->IsRtUsedAsDest)
            {
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Reorder = ROBEntry;
                pContext->RegisterStat[pInst->InstructionDWord.RType.Rt].Busy = true;
                pContext->ROB[ROBEntry].Dest = pInst->InstructionDWord.RType.Rt;
            }

          
            if (pInst->IsLoadOrStoreInst)
            {
                pContext->RS[RSEntry].Addr = pInst->InstructionDWord.IType.SignedImmediate;
            }

            if (pInst->IsBranchInst)
            {
                pContext->RS[RSEntry].Addr = pContext->RS[RSEntry].pInst->InstAddress + 4 + pInst->InstructionDWord.IType.SignedImmediate*4;
            }

            if (pInst->IsJumpInst)
            {
                pContext->RS[RSEntry].Addr = pInst->InstructionDWord.JType.Target;
            }
        }
                }
            }
        }
    }
}

void executeInst(MipsSimContext *pContext)
{
    UINT        CurrRSEntry = 0;
    Instruction *pInst = NULL;
    UINT        count = 0;
    bool        ComputeAddress = true;
    UINT        ROBCount = 0;
    UINT        RSCount = 0;
    //xin
    BTBEntry    *pBTBEntry = NULL;
    
    
    
    
    for (CurrRSEntry = 0; CurrRSEntry < MAX_RS_ENTRIES; CurrRSEntry++)
    {
        if (pContext->RS[CurrRSEntry].Busy && pContext->RS[CurrRSEntry].pInst->PipelineStage == INSTRUCTION_EX)
        {
            pInst = pContext->RS[CurrRSEntry].pInst;
            

            
            if ((pInst->IsRsUsedAsOperand && pInst->IsRtUsedAsOperand && 
                pContext->RS[CurrRSEntry].Qj == MAX_ROB_ENTRIES && pContext->RS[CurrRSEntry].Qk == MAX_ROB_ENTRIES) ||
                (pInst->IsRsUsedAsOperand && !pInst->IsRtUsedAsOperand && pContext->RS[CurrRSEntry].Qj == MAX_ROB_ENTRIES) ||
                (!pInst->IsRsUsedAsOperand && pInst->IsRtUsedAsOperand && pContext->RS[CurrRSEntry].Qk == MAX_ROB_ENTRIES) ||
                pInst->IsJumpInst)
            {
                
                switch (pInst->Opcode)
                {
                case OPCODE_SPECIAL:
                    switch (pInst->InstructionDWord.RType.Function)
                    {
                    case FUNC_SRL:
                    case FUNC_SRA:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj >> pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_SLL:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj << pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_ADD:
                    case FUNC_ADDU:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_SUB:
                    case FUNC_SUBU:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj - pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_AND:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj & pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_OR:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj | pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_XOR:
                        pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj ^ pContext->RS[CurrRSEntry].Vk;
                        break;

                    case FUNC_NOR:
                        pContext->RS[CurrRSEntry].Result = ~(pContext->RS[CurrRSEntry].Vj | pContext->RS[CurrRSEntry].Vk);
                        break;

                    case FUNC_SLT:
                    case FUNC_SLTU:
                        pContext->RS[CurrRSEntry].Result = (pContext->RS[CurrRSEntry].Vj < pContext->RS[CurrRSEntry].Vk) ? 1 : 0;
                        break;

                    default:
                        break;
                    }
                    break;

                case OPCODE_REGIMM:
                    switch (pInst->InstructionDWord.IType.Rt)
                    {
                    case RT_BLTZ:
                        if (pContext->RS[CurrRSEntry].Vj < 0)
                        {
                            // Branch Taken 
                            pInst->BranchOutcome = BRANCH_TAKEN;
                        }
                        else
                        {
                            // Branch Not Taken
                            pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                        }

                        pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                        break;

                    case RT_BGEZ:
                        if (pContext->RS[CurrRSEntry].Vj >= 0)
                        {
                            pInst->BranchOutcome = BRANCH_TAKEN;
                        }
                        else
                        {
                            pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                        }

                        pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                        break;

                    default:
                        break;
                    }

                case OPCODE_J:
                  
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BEQ:
                        
                        //xin
                        
                        if (checkInstructionInBTB(pContext, pInst->InstAddress) == NULL)
                        {
                            pBTBEntry = (BTBEntry*)malloc(sizeof(BTBEntry));
                            pBTBEntry->InstAddress = pInst->InstAddress;
                            if (pInst->IsJumpInst)
                            {
                                pBTBEntry->TargetAddress = pInst->InstructionDWord.JType.Target * 4;
                            }
                            else
                            {
                                pBTBEntry->TargetAddress = pInst->InstAddress + 4 + pInst->InstructionDWord.IType.SignedImmediate * 4;
                            }
                            pBTBEntry->PredState = BRANCH_NOT_TAKEN;
                            pBTBEntry->HitRate = 1;
                            pBTBEntry->next = NULL;
                            insertNewPredAtISStage(pContext, pBTBEntry);
                            
                            pInst->BranchPred = BRANCH_NOT_TAKEN;
                        }
                        
//
                        
                    if (pContext->RS[CurrRSEntry].Vj == pContext->RS[CurrRSEntry].Vk)
                    {
                        pInst->BranchOutcome = BRANCH_TAKEN;
                        //xin
                        //pInst->BranchPred = BRANCH_TAKEN;
                        //pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                        //
                    }
                    else
                    {
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BNE:
                    if (pContext->RS[CurrRSEntry].Vj != pContext->RS[CurrRSEntry].Vk)
                    {
                        pInst->BranchOutcome = BRANCH_TAKEN;
                    }
                    else
                    {
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BLEZ:
                    if (pContext->RS[CurrRSEntry].Vj <= 0)
                    {
                        pInst->BranchOutcome = BRANCH_TAKEN;
                    }
                    else
                    {
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_BGTZ:
                    if (pContext->RS[CurrRSEntry].Vj > 0)
                    {
                        pInst->BranchOutcome = BRANCH_TAKEN;
                    }
                    else
                    {
                        pInst->BranchOutcome = BRANCH_NOT_TAKEN;
                    }

                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    break;

                case OPCODE_ADDI:
                case OPCODE_ADDIU:
                    pContext->RS[CurrRSEntry].Result = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Vk;
                    break;

                case OPCODE_SLTI:
                    pContext->RS[CurrRSEntry].Result = (pContext->RS[CurrRSEntry].Vj < pContext->RS[CurrRSEntry].Vk) ? 1 : 0;
                    break;

                case OPCODE_LW:
                      
                        for (count = 0; count < MAX_ROB_ENTRIES; count++)
                        {
                            if (pContext->ROB[count].Busy &&
                                pContext->ROB[count].pInst != pInst &&
                                pContext->ROB[count].pInst->IsLoadOrStoreInst &&
                                pContext->ROB[count].pInst->InstIssueNum < pInst->InstIssueNum &&
                                !pContext->ROB[count].pInst->IsLoadOrStoreAddrComputed)
                            {
                                ComputeAddress = false;
                                break;
                            }
                        }

                        if (ComputeAddress)
                        {
                            // Proceed with Address Computation for Load
                            pContext->RS[CurrRSEntry].Addr = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Addr;
                            pInst->IsLoadOrStoreAddrComputed = true;
                        }
                    break;

                case OPCODE_SW:
                    
                    for (count = 0; count < MAX_ROB_ENTRIES; count++)
                    {
                        if (pContext->ROB[count].Busy &&
                            pContext->ROB[count].pInst->InstAddress != pInst->InstAddress &&
                            pContext->ROB[count].pInst->IsLoadOrStoreInst &&
                            pContext->ROB[count].pInst->InstIssueNum < pInst->InstIssueNum &&
                            !pContext->ROB[count].pInst->IsLoadOrStoreAddrComputed)
                        {
                            ComputeAddress = false;
                            break;
                        }
                    }

                    if (ComputeAddress)
                    {
                        // Proceed with Address Computation for Store 
                        pContext->RS[CurrRSEntry].Addr = pContext->RS[CurrRSEntry].Vj + pContext->RS[CurrRSEntry].Addr;
                        pInst->IsLoadOrStoreAddrComputed = true;
                    }

                    // Update the value in ROB
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Value = pContext->RS[CurrRSEntry].Vk;

                    // Mark the instruction to be ready
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Ready = true;

                    // Write the result of the Address to the ROB 
                    pContext->ROB[pContext->RS[CurrRSEntry].Dest].Addr = pContext->RS[CurrRSEntry].Addr;

                    break;
                    
                default:
                    break;
                }

                // Update PC here itself chiwanfan xiugai zheli
                if (pInst->IsBranchInst) /*|| pInst->IsJumpInst)*/
                {
                    if (pInst->BranchOutcome == BRANCH_NOT_TAKEN && pInst->BranchPred == BRANCH_TAKEN)
                    {
                        // Update PC to PC + 4
                        pContext->CurrentPC = pInst->InstAddress + 4;

                        // Cancel all Instructions fetched after Branch in ROB
                        for (ROBCount = 0; ROBCount < MAX_ROB_ENTRIES; ROBCount++)
                        {
                            if (pContext->ROB[ROBCount].pInst->InstIssueNum > pInst->InstIssueNum)
                            {
                                // Free up ROB and RS
                                pContext->ROB[ROBCount].UpdateBusyStatus = true;

                                for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
                                {
                                    if (pContext->RS[RSCount].Dest == ROBCount)
                                    {
                                        pContext->RS[RSCount].UpdateBusyStatus = true;
                                        break;
                                    }
                                }
                            }
                        }

                        // Also remove the last instruction from Instruction Queue
                        pContext->RemoveInstFromQueue = true;
                    }
                    //gai
/*
                    
 */
        //
                    if((pInst->BranchOutcome == BRANCH_TAKEN) && (pInst->BranchPred == BRANCH_TAKEN)){
                        // Update PC to new Target
                        pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                        // Also remove the last instruction from Instruction Queue
                        pContext->RemoveInstFromQueue = true;
                    }
                }
                /*
                //xin
                if(pInst->IsJumpInst){
                     pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                    // Also remove the last instruction from Instruction Queue
                    pContext->RemoveInstFromQueue = true;
                }
                //
                 */
                if (pInst->IsBranchInst /*|| pInst->IsJumpInst*/)
                {
                    pInst->UpdatePred = true;
                }

                // Move to the next stage in next cycle 
                pContext->RS[CurrRSEntry].pInst->UpdatePipelineStage = true;
            }
        }
    }
}

void writebackInst(MipsSimContext *pContext)
{
    UINT    currRSEntry = 0;
    UINT    count = 0;
    bool    AccessMem = true;

    // Check if there is a instruction in RS ready to be write back 
    for (currRSEntry = 0; currRSEntry < MAX_RS_ENTRIES; currRSEntry++)
    {
        if (pContext->RS[currRSEntry].Busy && pContext->RS[currRSEntry].pInst->PipelineStage == INSTRUCTION_WRITE_MEM)
        {
           
            // If its a load instruction check if the Mem Access Is Done
            if (pContext->RS[currRSEntry].pInst->Opcode == OPCODE_LW && !pContext->RS[currRSEntry].pInst->IsLoadMemAccessDone)
            {
                // Do the memory access in this cycle
                // Load takes 2 cycles in write back stage 
                // First Check if there is no early store with the same address 
                for (count = 0; count < MAX_ROB_ENTRIES; count++)
                {
                    if (pContext->ROB[count].Busy &&
                        pContext->ROB[count].pInst != pContext->RS[currRSEntry].pInst &&
                        pContext->ROB[count].pInst->Opcode == OPCODE_SW &&
                        pContext->ROB[count].pInst->InstIssueNum < pContext->RS[currRSEntry].pInst->InstIssueNum &&
                        !pContext->ROB[count].Addr == pContext->RS[currRSEntry].Addr)
                    {
                        AccessMem = false;
                    }
                }

                if (AccessMem)
                {
                    pContext->RS[currRSEntry].Result = getDataFromList(pContext, pContext->RS[currRSEntry].Addr)->DataDWord;
                }
                
                pContext->RS[currRSEntry].pInst->IsLoadMemAccessDone = true;
                continue;
            }

            // Check if any RS is waiting for this result 
            for (count = 0; count < MAX_RS_ENTRIES; count++)
            {
                // Update Qj if any instruction is waiting for result
                if (pContext->RS[count].Qj == pContext->RS[currRSEntry].Dest)
                {
                    pContext->RS[count].Vj = pContext->RS[currRSEntry].Result;
                    pContext->RS[count].Qj = MAX_ROB_ENTRIES;
                }

                // Update Qk if any instruction is waiting for result
                if (pContext->RS[count].Qk == pContext->RS[currRSEntry].Dest)
                {
                    pContext->RS[count].Vk = pContext->RS[currRSEntry].Result;
                    pContext->RS[count].Qk = MAX_ROB_ENTRIES;
                }
            }

            // Update the ROB with the result and make it ready state
            pContext->ROB[pContext->RS[currRSEntry].Dest].Value = pContext->RS[currRSEntry].Result;
            pContext->ROB[pContext->RS[currRSEntry].Dest].Ready = true;
        
            // Move Inst to next stage 
            pContext->RS[currRSEntry].pInst->UpdatePipelineStage = true;
            
            
        }
        
        
        
        
    }
}

void commitInst(MipsSimContext *pContext)
{
    UINT    ROBEntry = 0;
    Data*   pData = NULL;
    UINT    ROBCount = 0, RSCount = 0;
    Instruction *pInst = NULL;

    // Check that ROB is not empty!
    if (!IsROBEmpty(pContext))
    {
        // Get the oldest instruction in the ROB to commit
        ROBEntry = getROBEntryOldestInst(pContext);

        // Check if the Inst is in the commit stage  
        if (pContext->ROB[ROBEntry].pInst->PipelineStage == INSTRUCTION_COMMIT)
        {
            /*
            //xin
            pInst = pContext->ROB[ROBEntry].pInst;
            printf("%d\n",pInst->InstAddress);
            
            if (pContext->ROB[ROBEntry].pInst->Opcode == OPCODE_J){
                pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                printf("%d\n",pInst->InstAddress);

                BTBEntry    *pBTBEntry = NULL;
                
                if (checkInstructionInBTB(pContext, pInst->InstAddress) == NULL)
                {
                    pBTBEntry = (BTBEntry*)malloc(sizeof(BTBEntry));
                    pBTBEntry->InstAddress = pInst->InstAddress;
                    if (pInst->IsJumpInst)
                    {
                        pBTBEntry->TargetAddress = pInst->InstructionDWord.JType.Target * 4;
                    }
                    else
                    {
                        pBTBEntry->TargetAddress = pInst->InstAddress + 4 + pInst->InstructionDWord.IType.SignedImmediate * 4;
                    }
                    //pBTBEntry->PredState = NOT_SET;
                    pBTBEntry->PredState = BRANCH_TAKEN;
                    pBTBEntry->HitRate = 1;
                    pBTBEntry->next = NULL;
                    insertNewPredAtISStage(pContext, pBTBEntry);
                    
                    pInst->BranchPred = BRANCH_TAKEN;
                }

            }
         
            //
*/
            if (pContext->ROB[ROBEntry].pInst->Opcode == OPCODE_SW)
            {
                // Update the mem location with the value
                pData = getDataFromList(pContext, pContext->ROB[ROBEntry].Addr);
                pData->DataDWord = pContext->ROB[ROBEntry].Value;
                setDataToList(pContext, pData);
            }
            //xin
            else if(pContext->ROB[ROBEntry].pInst->Opcode == OPCODE_BEQ){
                pInst = pContext->ROB[ROBEntry].pInst;
                if (pInst->BranchOutcome == BRANCH_TAKEN && (pInst->BranchPred == BRANCH_NOT_TAKEN || pInst->BranchPred == NOT_SET))
                {
                    // Update PC to new Target
                    pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                    
                    // Cancel all Instructions fetched after Branch in ROB
                    for (ROBCount = 0; ROBCount < MAX_ROB_ENTRIES; ROBCount++)
                    {
                        if (pContext->ROB[ROBCount].pInst->InstIssueNum > pInst->InstIssueNum)
                        {
                            // Free up ROB and RS
                            pContext->ROB[ROBCount].UpdateBusyStatus = true;
                            
                            for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
                            {
                                if (pContext->RS[RSCount].Dest == ROBCount)
                                {
                                    pContext->RS[RSCount].UpdateBusyStatus = true;
                                    break;
                                }
                            }
                        }
                    }
                    
                    // Also remove the last instruction from Instruction Queue
                    pContext->RemoveInstFromQueue = true;
                    a = 3;
                    b = 3;
                }
                
            }
            
            //
            
            //xin
            else if(pContext->ROB[ROBEntry].pInst->Opcode == OPCODE_J){
                
                pInst = pContext->ROB[ROBEntry].pInst;
                //pInst->BranchOutcome = BRANCH_TAKEN;
                printf("%d\n",pInst->InstAddress);
                //pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                BTBEntry    *pBTBEntry = NULL;
                
                if (checkInstructionInBTB(pContext, pInst->InstAddress) == NULL)
                {
                    pBTBEntry = (BTBEntry*)malloc(sizeof(BTBEntry));
                    pBTBEntry->InstAddress = pInst->InstAddress;
                    if (pInst->IsJumpInst)
                    {
                        pBTBEntry->TargetAddress = pInst->InstructionDWord.JType.Target * 4;
                    }
                    else
                    {
                        pBTBEntry->TargetAddress = pInst->InstAddress + 4 + pInst->InstructionDWord.IType.SignedImmediate * 4;
                    }
                    //pBTBEntry->PredState = NOT_SET;
                    pBTBEntry->PredState = BRANCH_TAKEN;
                    pBTBEntry->HitRate = 1;
                    pBTBEntry->next = NULL;
                    insertNewPredAtISStage(pContext, pBTBEntry);
                    
                    pInst->BranchPred = BRANCH_TAKEN;
                    
                    
                    //xin
                    // Cancel all Instructions fetched after Branch in ROB
                    for (ROBCount = 0; ROBCount < MAX_ROB_ENTRIES; ROBCount++)
                    {
                        if (pContext->ROB[ROBCount].pInst->InstIssueNum > pInst->InstIssueNum)
                        {
                            // Free up ROB and RS
                            pContext->ROB[ROBCount].UpdateBusyStatus = true;
                            
                            for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
                            {
                                if (pContext->RS[RSCount].Dest == ROBCount)
                                {
                                    pContext->RS[RSCount].UpdateBusyStatus = true;
                                    break;
                                }
                            }
                        }
                    }
                    //
                    //XIN
                    a = 3;
                    b = 3;
                    //
                    pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                }
                //pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
                
            }
            //
            
            else
            {
                if (!pContext->ROB[ROBEntry].pInst->IsBranchInst && !pContext->ROB[ROBEntry].pInst->IsJumpInst && !pContext->ROB[ROBEntry].pInst->IsBreakOrNopInst)
                {
                    // Update the destination register 
                    RegLookUpTable[pContext->ROB[ROBEntry].Dest].RegValue = pContext->ROB[ROBEntry].Value;
                }
            }

            // Free up the ROB Entry & corresponding RS Entry
            pContext->ROB[ROBEntry].UpdateBusyStatus = true;
            
            for (RSCount = 0; RSCount < MAX_RS_ENTRIES; RSCount++)
            {
                if (pContext->RS[RSCount].Dest == ROBEntry)
                {
                    pContext->RS[RSCount].UpdateBusyStatus = true;
                    break;
                }
            }

            // Free up the register
            if (pContext->RegisterStat[pContext->ROB[ROBEntry].Dest].Reorder == ROBEntry)
            {
                pContext->RegisterStat[pContext->ROB[ROBEntry].Dest].Busy = false;
            }
        }
    }
}

void simulateExTomasulo(MipsSimContext *pContext)
{
    UINT count = 0;
    
    // Keep Executing till IQ is empty and all the ROB entries are free ! 
    do
    {
        // Increment Cycle No 
        pContext->CurrentCycle++;

//        if (pContext->CurrentCycle == 45) break;

        // I. Instruction Fetch
        fetchInst(pContext);

        // II. Instruction Issue/Decode 
        issueInst(pContext);

        // III. Instruction Execute
        executeInst(pContext);
        
        // IV. Instruction Write Result
        writebackInst(pContext);

        if (!pContext->Args.PrintFinalState)
        {
            // Print the Cycle output in the output txt file
            printOutputFile(pContext);
        }

        // V. Instruction Commit
        //commitInst(pContext);

        // End of Cycle: Update the Instruction Pipeline Stage in RS
        for (count = 0; count < MAX_ROB_ENTRIES; count++)
        {
            if (pContext->ROB[count].Busy && pContext->ROB[count].pInst->UpdatePipelineStage)
            {
                // if there is a NOP or BREAK Instruction in ISSUE stage, update it directly to commit stage 
                if (pContext->ROB[count].pInst->IsBreakOrNopInst && pContext->ROB[count].pInst->PipelineStage == INSTRUCTION_ISSUE)
                {
                    pContext->ROB[count].pInst->PipelineStage = INSTRUCTION_COMMIT;
                }
                // SW, Jump or Branch dont have a write back stage, directly move them to Commit 
                else if ((pContext->ROB[count].pInst->IsBranchInst || pContext->ROB[count].pInst->IsJumpInst || pContext->ROB[count].pInst->Opcode == OPCODE_SW) &&
                    pContext->ROB[count].pInst->PipelineStage == INSTRUCTION_EX)
                {
                    pContext->ROB[count].pInst->PipelineStage = INSTRUCTION_COMMIT;
                }
                //else if(pContext->ROB[count].pInst->PipelineStage == INSTRUCTION_EX){
                //    pContext->ROB[count].pInst->PipelineStage = INSTRUCTION_COMMIT;
                //}
                else
                {
                    // Move to the next Pipeline Stage
                    pContext->ROB[count].pInst->PipelineStage++;
                }
                pContext->ROB[count].pInst->UpdatePipelineStage = false;
            }
        }
        //V. Instruction Commit
        commitInst(pContext);
        
        // Update the Busy status for RS entries 
        for (count = 0; count < MAX_RS_ENTRIES; count++)
        {
            if (pContext->RS[count].Busy)
            {
                if (pContext->RS[count].pInst->UpdatePred)
                {
                    updatePredAtEXStage(pContext, pContext->RS[count].pInst->InstAddress, pContext->RS[count].pInst->BranchOutcome);
                    pContext->RS[count].pInst->UpdatePred = false;
                }
            }

            if (pContext->RS[count].UpdateBusyStatus)
            {
                pContext->RS[count].Busy = false;
                pContext->RS[count].UpdateBusyStatus = false;
            }
        }

        // Update the Busy status for ROB entries 
        for (count = 0; count < MAX_ROB_ENTRIES; count++)
        {
            if (pContext->ROB[count].UpdateBusyStatus)
            {
                pContext->ROB[count].Busy = false;
                c = 2;
                pContext->ROB[count].UpdateBusyStatus = false;
            }
        }

        // Remove Inst from Queue if required 
        if (pContext->RemoveInstFromQueue)
        {
            pContext->InstQueue = NULL;
            pContext->RemoveInstFromQueue = false;
        }
       
        //xin
        if(b == 3){
            b = 0;
            pContext->CurrentCycle++;
            printOutputFile(pContext);
        }
        
        //
    } while (!IsSimComplete(pContext));
    

    // Print one more cycle
    pContext->CurrentCycle++;

    // Print the Cycle output in the output txt file
    printOutputFile(pContext);
    /*
    if(b == 3){
        Instruction *pInst = NULL;
        pInst = getInstFromList(pContext, pContext->CurrentPC);
        pContext->CurrentPC = checkInstructionInBTB(pContext, pInst->InstAddress)->TargetAddress;
        b = 0;
    }
*/

}

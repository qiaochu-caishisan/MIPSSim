
/************************************************************************/
/*btb.h                                                                 */
/*Authorized by: Qiao Chu                                               */
/************************************************************************/

#ifndef BTB_H_
#define BTB_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "InstParser.h"

#define BTB_MAX_SIZE 16


//BTB table
typedef struct _BTBEntry
{
    UINT    InstAddress;
    UINT    TargetAddress;
    UINT    PredState;
    struct  _BTBEntry *next;
    UINT    HitRate;
}BTBEntry, *PBTBEntry;


typedef enum _PredictorState
{
    NOT_SET,
    BRANCH_TAKEN,
    BRANCH_NOT_TAKEN,
}PredictorState;



#endif

#include "stats.h"

#include "stdint.h"
#include "stdio.h"

const Stats statsDefault = 
{ 
    .min = INT32_MAX, 
    .max = INT32_MIN, 
    .avg = 0,
    .avg_sum = 0,
    .collectionIt = 0
};

void statsReset(Stats *pStats)
{
    *pStats = statsDefault;
}

void statsCollect(int32_t value, Stats * pStats)
{
    pStats->collectionIt++;
    pStats->avg_sum += value;
    if(value < pStats->min) pStats->min = value;  
    if(value > pStats->max) pStats->max = value;   
}

void statsUpdate(Stats *pNew, Stats *pOld)
{ 
    pNew->avg = pNew->avg_sum / pNew->collectionIt;
    *pOld = *pNew;
    statsReset(pNew);
}

void statsToString(Stats stats, char * pStatName, char * pString)
{
    sprintf(pString, "%s: max=%d, min=%d, avg=%d", 
        pStatName, stats.max, stats.min, stats.avg);
}

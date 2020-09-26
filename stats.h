#ifndef STATS_H
#define STATS_H

#include <stdint.h>

typedef struct Stats_
{
    int32_t min;
    int32_t max;
    int32_t avg;

    int64_t avg_sum;
    uint32_t collectionIt;
} Stats;

extern const Stats statsDefault;

void statsReset(Stats *pStats);
void statsCollect(int32_t value, Stats * pStats);
void statsUpdate(Stats * pNew, Stats * pOld);
void statsDraw(Stats stats, char * pStatName, uint16_t x, uint16_t y);
void statsToString(Stats stats, char * pStatName, char * pString);

#endif //STATS_H

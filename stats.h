#ifndef STATS_H
#define STATS_H

#include <stdint.h>

typedef struct Stats_
{
	int64_t min;
	int64_t max;
	int64_t avg;

	int64_t avg_sum;
	uint64_t collectionIt;
} Stats;

extern const Stats statsDefault;

void statsReset(Stats *pStats);
void statsCollect(int64_t value, Stats * pStats);
void statsRecord(Stats * pNew, Stats * pOld);
int64_t statsGetMax(Stats stats);
int64_t statsGetMin(Stats stats);
int64_t statsGetAvg(Stats stats);
void statsToString(Stats stats, char * pStatName, char * pString);

#endif //STATS_H

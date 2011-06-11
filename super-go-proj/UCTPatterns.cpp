#include "UCTPatterns.h"

bool UctPatterns::centerCode[2][UctPatterns::POWER3_9];

bool UctPatterns::edgeCode[2][UctPatterns::POWER3_9];

int UctPatterns::up[SG_MAXPOINT];

int UctPatterns::pos[SG_MAXPOINT];

int UctPatterns::line[SG_MAXPOINT];

bool UctPatterns::initialized = false;


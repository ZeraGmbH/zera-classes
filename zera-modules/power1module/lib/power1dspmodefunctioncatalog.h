#ifndef POWER1DSPMODEFUNCTIONCATALOG_H
#define POWER1DSPMODEFUNCTIONCATALOG_H

#include "power1dspcmdgenerator.h"
#include "measmodebroker.h"

class Power1DspModeFunctionCatalog
{
public:
    static const ModeNameFunctionHash get(int measSytemCount);
};

#endif // POWER1DSPMODEFUNCTIONCATALOG_H

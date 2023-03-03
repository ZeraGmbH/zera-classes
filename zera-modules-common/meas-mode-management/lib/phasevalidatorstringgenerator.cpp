#include "phasevalidatorstringgenerator.h"
#include <math.h>

QStringList PhaseValidatorStringGenerator::generate(int phaseCount, int maxPhaseCount)
{
    QStringList ret;
    if(phaseCount <= maxPhaseCount) {
        for(int cases=0; cases<pow(2, phaseCount); cases++) {
            QString singleCaseList;
            for(int phase=0; phase<phaseCount; phase++)
                singleCaseList.append(cases & (1<<phase) ? "1" : "0");
            if(!singleCaseList.isEmpty())
                ret.append(singleCaseList);
        }
    }
    return ret;
}

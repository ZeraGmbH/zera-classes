#ifndef PHASEVALIDATORSTRINGGENERATOR_H
#define PHASEVALIDATORSTRINGGENERATOR_H

#include <QStringList>

class PhaseValidatorStringGenerator
{
public:
    static QStringList generate(int phaseCount, int maxPhaseCount=3);
};

#endif // PHASEVALIDATORSTRINGGENERATOR_H

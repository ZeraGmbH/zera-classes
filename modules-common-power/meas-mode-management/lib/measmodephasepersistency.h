#ifndef MEASMODEPHASEPERSISTENCY_H
#define MEASMODEPHASEPERSISTENCY_H

#include "measmode.h"

class MeasModePhasePersistency
{
public:
    static bool setMeasModePhaseFromConfig(MeasModePtr mode, QStringList modePhaseListFromConfig);
};

#endif // MEASMODEPHASEPERSISTENCY_H

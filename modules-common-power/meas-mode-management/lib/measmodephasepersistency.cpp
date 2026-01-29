#include "measmodephasepersistency.h"
#include "measmodemaskstringconverter.h"

bool MeasModePhasePersistency::setMeasModePhaseFromConfig(MeasModePtr mode, const QStringList &modePhaseListFromConfig)
{
    for(auto& entry : modePhaseListFromConfig) {
        QStringList modePhaseList = entry.split(',');
        if(modePhaseList.count() != 2)
            return false;
        QString name = modePhaseList[0];
        QString mask = modePhaseList[1];
        if(name == mode->getName()) {
            MModePhaseMask binMask;
            if(!MeasModeMaskStringConverter::calcBinMask(mask, mode->getMeasSysCount(), binMask))
                return false;
            return mode->tryChangeMask(mask);
        }
    }
    return false;
}

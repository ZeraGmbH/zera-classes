#include "measmodephasepersistency.h"
#include "measmodemaskstringconverter.h"

bool MeasModePhasePersistency::setMeasModePhaseFromConfig(MeasModePtr mode, const QStringList &modePhaseListFromConfig)
{
    for(const auto& entry : modePhaseListFromConfig) {
        QStringList modePhaseList = entry.split(',');
        if(modePhaseList.count() != 2)
            return false;
        QString name = modePhaseList[0];
        if(name == mode->getName()) {
            MModePhaseMask binMask;
            QString mask = modePhaseList[1];
            if(!MeasModeMaskStringConverter::calcBinMask(mask, mode->getMeasSysCount(), binMask))
                return false;
            return mode->tryChangeMask(mask);
        }
    }
    return false;
}

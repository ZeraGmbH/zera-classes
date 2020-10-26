#ifndef SEC1MODULECONFIGDATA_H
#define SEC1MODULECONFIGDATA_H

#include <QString>
#include <QList>

#include "socket.h"

namespace SEC1MODULE
{

// used for configuration export


struct boolParameter
{
    QString m_sKey;
    quint8 m_nActive; // active or 1,0
};


struct doubleParameter
{
    QString m_sKey;
    double m_fPar;
};


struct intParameter
{
    QString m_sKey;
    quint32 m_nPar;
};


struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};


class cSec1ModuleConfigData
{
public:
    cSec1ModuleConfigData(){}
    quint8 m_nDebugLevel;
    quint8 m_nRefInpCount;
    quint8 m_nDutInpCount;
    quint8 m_nModeCount;
    stringParameter m_sRefInput;
    stringParameter m_sDutInput;
    QList<QString> m_refInpList;
    QList<QString> m_dutInpList;
    QList<QString> m_ModeList;
    bool m_bEmbedded;
    stringParameter m_sMode;
    double m_fMeasInterval; // measuring interval 0.2 .. 5.0 sec. (time between actualizations)
    doubleParameter m_fDutConstant;
    doubleParameter m_fRefConstant;
    boolParameter m_bContinous;
    doubleParameter m_fT0Value;
    doubleParameter m_fT1Value;
    intParameter m_nTarget;
    doubleParameter m_fEnergy;
    intParameter m_nMRate;
    doubleParameter m_fUpperLimit;
    doubleParameter m_fLowerLimit;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_SECServerSocket;
};

}

#endif // SEC1MODULECONFIGDATA_H

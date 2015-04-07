#ifndef SEC1MODULECONFIGDATA_H
#define SEC1MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"

namespace SEC1MODULE
{

// used for configuration export


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
    bool m_bwithrefconstantnotifier;
    stringParameter m_sRefInput;
    stringParameter m_sDutInput;
    QStringList m_refInpList;
    QStringList m_dutInpList;
    double m_fMeasInterval; // measuring interval 0.2 .. 5.0 sec. (time between actualizations)
    doubleParameter m_fDutConstant;
    doubleParameter m_fRefConstant;
    intParameter m_nTargetValue;
    intParameter m_nMeasPulses;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_SECServerSocket;
};

}

#endif // SEC1MODULECONFIGDATA_H

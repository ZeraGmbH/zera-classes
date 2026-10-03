#ifndef SCPIMODULECONFIGDATA_H
#define SCPIMODULECONFIGDATA_H

#include "configtypes.h"
#include "networkconnectioninfo.h"
#include "statusbitdescriptor.h"
#include <QStringList>

namespace SCPIMODULE
{

struct serialDevice
{
    quint8 m_nOn = 0;
    quint32 m_nBaud = 0;
    quint8 m_nStopbits = 0;
    quint8 m_nDatabits = 0;
    QString m_sDevice;
};

class cSCPIModuleConfigData
{
public:
    cSCPIModuleConfigData(){}
    quint8 m_nClients = 0; // the max. nr of clients accepted to connect
    NetworkConnectionInfo m_InterfaceSocket; // we listen here ip is localhost
    QString m_sDeviceName;
    quint8 m_nQuestonionableStatusBitCount = 0;
    quint8 m_nOperationStatusBitCount = 0;
    quint8 m_nOperationMeasureStatusBitCount = 0;
    QList<cStatusBitDescriptor> m_QuestionableStatDescriptorList;
    QList<cStatusBitDescriptor> m_OperationStatDescriptorList;
    QList<cStatusBitDescriptor> m_OperationMeasureStatDescriptorList;
    serialDevice m_SerialDevice;
    boolParameter m_queryResponseSortActive;
};

}

#endif // SCPIMODULECONFIGDATA_H

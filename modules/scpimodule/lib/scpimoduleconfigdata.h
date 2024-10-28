#ifndef SCPIMODULECONFIGDATA_H
#define SCPIMODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"
#include "statusbitdescriptor.h"


namespace SCPIMODULE
{

struct boolParameter
{
    QString m_sKey;
    quint8 m_nActive; // active or not 1,0
};

struct serialDevice
{
    quint8 m_nOn;
    quint32 m_nBaud;
    quint8 m_nStopbits;
    quint8 m_nDatabits;
    QString m_sDevice;
};

class cSCPIModuleConfigData
{
public:
    cSCPIModuleConfigData(){}
    quint8 m_nClients; // the max. nr of clients accepted to connect
    cSocket m_InterfaceSocket; // we listen here ip is localhost
    QString m_sDeviceName;
    quint8 m_nQuestonionableStatusBitCount;
    quint8 m_nOperationStatusBitCount;
    quint8 m_nOperationMeasureStatusBitCount;
    QList<cStatusBitDescriptor> m_QuestionableStatDescriptorList;
    QList<cStatusBitDescriptor> m_OperationStatDescriptorList;
    QList<cStatusBitDescriptor> m_OperationMeasureStatDescriptorList;
    serialDevice m_SerialDevice;
    boolParameter m_enableScpiQueue;
};

}

#endif // SCPIMODULECONFIGDATA_H

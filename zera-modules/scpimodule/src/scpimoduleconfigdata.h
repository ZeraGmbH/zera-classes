#ifndef SCPIMODULECONFIGDATA_H
#define SCPIMODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"
#include "statusbitdescriptor.h"


namespace SCPIMODULE
{


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
    quint8 m_nDebugLevel;
    quint8 m_nClients; // the max. nr of clients accepted to connect
    cSocket m_InterfaceSocket; // we listen here ip is localhost
    QString m_sDeviceName;
    QString m_sDeviceIdentification;
    quint8 m_nQuestonionableStatusBitCount;
    quint8 m_nOperationStatusBitCount;
    quint8 m_nOperationMeasureStatusBitCount;
    QList<statusBitDescriptor> m_QuestionableStatDescriptorList;
    QList<statusBitDescriptor> m_OperationStatDescriptorList;
    QList<statusBitDescriptor> m_OperationMeasureStatDescriptorList;
    serialDevice m_SerialDevice;
};

}

#endif // SCPIMODULECONFIGDATA_H

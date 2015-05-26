#ifndef SCPIMODULECONFIGDATA_H
#define SCPIMODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"

namespace SCPIMODULE
{

// used for configuration export


struct statusBitDescriptor
{
    quint8 m_nBitNr;
    QString m_sModule;
    QString m_sEntity;
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
};

}

#endif // SCPIMODULECONFIGDATA_H

#ifndef SOURCEDEVICEBASE_H
#define SOURCEDEVICEBASE_H

#include "sourceinterface.h"
#include "supportedsources.h"
#include "sourceiopacketgenerator.h"

#include <QObject>

namespace SOURCEMODULE
{
class SourceDeviceBase : public QObject
{
    Q_OBJECT
public:
    explicit SourceDeviceBase(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version);
    virtual ~SourceDeviceBase();

    // requests

    // getter
    bool isDemo();
    QString getInterfaceDeviceInfo();

protected:
    tSourceInterfaceShPtr m_ioInterface;
    cSourceIoPacketGenerator* m_outInGenerator = nullptr;

};
}

#endif // SOURCEDEVICEBASE_H

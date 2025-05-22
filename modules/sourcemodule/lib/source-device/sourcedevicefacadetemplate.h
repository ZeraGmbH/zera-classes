#ifndef SOURCEDEVICEFACADETEMPLATE_H
#define SOURCEDEVICEFACADETEMPLATE_H

#include "iodevicebase.h"
#include "idgenerator.h"
#include "sourceveininterface.h"
#include "sourcestatecontroller.h"
#include "jsondevicestatusapi.h"

class SourceDeviceFacadeTemplate : public QObject
{
    Q_OBJECT
public:
    SourceDeviceFacadeTemplate(IoDeviceBase::Ptr ioDevice);
    virtual void setVeinInterface(SourceVeinInterface* veinInterface) = 0;
    virtual void setStatusPollTime(int ms) = 0;
    virtual void switchLoad(QJsonObject params) = 0;

    int getId();
    QString getIoDeviceInfo() const;
    bool hasDemoIo() const;
    QStringList getLastErrors() const;

    virtual bool close(QUuid uuid) = 0;
signals:
    void sigClosed(int facadeId, QUuid uuid);

protected:
    void handleErrorState(SourceStateController::States state);
    void setVeinParamStructure(QJsonObject paramStruct);
    void setVeinDeviceState(QJsonObject deviceState);
    void setVeinParamState(QJsonObject paramState);
    void resetVeinComponents();

    IoDeviceBase::Ptr m_ioDevice;
    SourceVeinInterface* m_veinInterface = nullptr;
    JsonDeviceStatusApi m_deviceStatusJsonApi;
private:
    static IoIdGenerator m_idGenerator;
    int m_ID;
};

#endif // SOURCEDEVICEFACADETEMPLATE_H

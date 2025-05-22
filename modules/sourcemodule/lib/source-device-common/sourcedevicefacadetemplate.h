#ifndef SOURCEDEVICEFACADETEMPLATE_H
#define SOURCEDEVICEFACADETEMPLATE_H

#include "iodevicebase.h"
#include "idgenerator.h"
#include "sourceveininterface.h"
#include "sourcestatecontroller.h"
#include "jsondevicestatusapi.h"
#include "abstractsourceswitchjson.h"

class SourceDeviceFacadeTemplate : public QObject
{
    Q_OBJECT
public:
    SourceDeviceFacadeTemplate(IoDeviceBase::Ptr ioDevice, ISourceIo::Ptr sourceIo);
    void setVeinInterface(SourceVeinInterface* veinInterface);

    virtual void setStatusPollTime(int ms) = 0;
    void switchLoad(QJsonObject params);

    int getId();
    QString getIoDeviceInfo() const;
    bool hasDemoIo() const;
    QStringList getLastErrors() const;

    virtual bool close(QUuid uuid) = 0;
signals:
    void sigClosed(int facadeId, QUuid uuid);

protected slots:
    void onSourceStateChanged(SourceStateController::States state);
protected:
    void handleErrorState(SourceStateController::States state);
    void setVeinParamStructure(QJsonObject paramStruct);
    void setVeinDeviceState(QJsonObject deviceState);
    void setVeinParamState(QJsonObject paramState);
    void resetVeinComponents();

    IoDeviceBase::Ptr m_ioDevice;
    SourceVeinInterface* m_veinInterface = nullptr;
    JsonDeviceStatusApi m_deviceStatusJsonApi;
    std::unique_ptr<AbstractSourceSwitchJson> m_switcher;
    ISourceIo::Ptr m_sourceIo;
private:
    static IoIdGenerator m_idGenerator;
    int m_ID;
};

#endif // SOURCEDEVICEFACADETEMPLATE_H

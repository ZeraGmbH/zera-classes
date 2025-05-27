#ifndef SOURCEDEVICETEMPLATE_H
#define SOURCEDEVICETEMPLATE_H

#include "iodevicebase.h"
#include "idgenerator.h"
#include "sourceveininterface.h"
#include "sourcestatecontroller.h"
#include "jsondevicestatusapi.h"
#include "abstractsourceswitchjson.h"
#include <QUuid>
#include <memory>

class SourceDeviceTemplate : public QObject
{
    Q_OBJECT
public:
    typedef std::shared_ptr<SourceDeviceTemplate> Ptr;
    SourceDeviceTemplate(IoDeviceBase::Ptr ioDevice, const QJsonObject &sourceJsonStruct);

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

    const QJsonObject m_sourceJsonStruct;
    IoDeviceBase::Ptr m_ioDevice;
    SourceVeinInterface* m_veinInterface = nullptr;
    JsonDeviceStatusApi m_deviceStatusJsonApi;
    std::unique_ptr<AbstractSourceSwitchJson> m_switcher;
private:
    static IoIdGenerator m_idGenerator;
    int m_ID;
};

#endif // SOURCEDEVICETEMPLATE_H

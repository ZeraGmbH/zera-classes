#ifndef SOURCEDEVICETEMPLATE_H
#define SOURCEDEVICETEMPLATE_H

#include "idgenerator.h"
#include "iodevicetypes.h"
#include "sourcestates.h"
#include "sourceveininterface.h"
#include "jsondevicestatusapi.h"
#include "abstractsourceswitchjson.h"
#include <QUuid>
#include <memory>

class SourceDeviceTemplate : public QObject
{
    Q_OBJECT
public:
    typedef std::shared_ptr<SourceDeviceTemplate> Ptr;
    SourceDeviceTemplate(QString deviceInfo, IoDeviceTypes deviceType, const QJsonObject &sourceCapabilities);

    void setVeinInterface(SourceVeinInterface* veinInterface);

    void switchLoad(QJsonObject params);

    int getId();
    const QString &getIoDeviceInfo() const;
    bool hasDemoIo() const;
    QStringList getLastErrors() const;

    virtual bool close(QUuid uuid) = 0;
signals:
    void sigClosed(int facadeId, QUuid uuid);

protected slots:
    void handleNewState(SourceStates state);
protected:
    void handleErrorState(SourceStates state);
    void setVeinParamStructure(QJsonObject sourceCapabilities);
    void setVeinDeviceState(QJsonObject deviceState);
    void setVeinParamState(QJsonObject paramState);
    void resetVeinComponents();

    const QJsonObject m_sourceCapabilities;
    IoDeviceTypes m_deviceType;
    QString m_deviceInfo;
    SourceVeinInterface* m_veinInterface = nullptr;
    JsonDeviceStatusApi m_deviceStatusJsonApi;
    std::unique_ptr<AbstractSourceSwitchJson> m_switcher;
private:
    static IoIdGenerator m_idGenerator;
    int m_ID;
};

#endif // SOURCEDEVICETEMPLATE_H

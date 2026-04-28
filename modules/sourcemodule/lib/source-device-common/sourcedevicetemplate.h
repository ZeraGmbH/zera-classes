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
    SourceDeviceTemplate(const QString &deviceName, IoDeviceTypes deviceType, const QJsonObject &sourceCapabilities);

    void setVeinInterface(SourceVeinInterface* veinInterface);

    void switchLoad(const QJsonObject &params);

    int getId();
    const QString &getDeviceInfo() const;
    bool hasDemoIo() const;
    QStringList getLastErrors() const;

    virtual bool close(QUuid uuid) = 0;
public slots:
    void handleNewState(SourceStates state);
signals:
    void sigClosed(int facadeId, QUuid uuid);

protected:
    void setVeinLoadpointParam(QJsonObject loadpointParam);
    void resetVeinComponents();

    const QJsonObject m_sourceCapabilities;
    std::unique_ptr<AbstractSourceSwitchJson> m_switcher;
private:
    void handleErrorState(SourceStates state);
    void setVeinParamStructure(QJsonObject sourceCapabilities);
    void setVeinDeviceState(QJsonObject deviceState);

    IoDeviceTypes m_deviceType;
    QString m_deviceName;
    SourceVeinInterface* m_veinInterface = nullptr;
    JsonDeviceStatusApi m_deviceStatusJsonApi;

    bool m_lastOnState = false;
    static IoIdGenerator m_idGenerator; // ID for device manager
    int m_ID;
};

#endif // SOURCEDEVICETEMPLATE_H

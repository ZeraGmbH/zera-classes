#ifndef SOURCEDEVICE_H
#define SOURCEDEVICE_H

#include "sourcedevicesubject.h"
#include "../supportedsources.h"
#include "../sourceioworker.h"

#include <QObject>

/*
 * Start cmd worker on external demand
 * Notify observers attached on commnd response
 * Keep source information
 * Allow adjustments on demo interfaces
 */
class SourceDevice : public QObject, public SourceDeviceSubject
{
    Q_OBJECT
public:
    SourceDevice(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version);
    ~SourceDevice();

    // requests
    int startTransaction(const SourceWorkerCmdPack &workerPack);
    void simulateExternalDisconnect();

    // setter
    void setDemoResponseDelay(bool followsTimeout, int fixedMs);

    // getter
    SupportedSourceTypes getType() const;
    QString getName() const;
    QString getVersion() const;
    QString getInterfaceInfo() const;
    bool isDemo() const;

signals:
    void sigInterfaceDisconnected();

private slots:
    void onSourceCmdFinished(SourceWorkerCmdPack cmdPack);

private:
    tSourceInterfaceShPtr m_ioInterface;
    SourceIoWorker m_sourceIoWorker;

    SupportedSourceTypes m_type;
    QString m_name;
    QString m_version;

    bool m_demoDelayFollowsTimeout = false;
    int m_demoDelayFixedMs = 0;
};

#endif // SOURCEDEVICE_H

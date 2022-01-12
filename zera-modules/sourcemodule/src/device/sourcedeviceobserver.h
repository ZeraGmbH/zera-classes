#ifndef SOURCEDEVICEOBSERVER_H
#define SOURCEDEVICEOBSERVER_H

#include <QObject>

class SourceWorkerCmdPack;
class SourceDeviceSubject;


class SourceDeviceObserver : public QObject
{
    Q_OBJECT
public:
    SourceDeviceObserver(SourceDeviceSubject* subject);

protected slots:
    virtual void updateResponse(SourceWorkerCmdPack cmdPack) = 0;
    friend class SourceDeviceSubject;
};

#endif // SOURCEDEVICEOBSERVER_H

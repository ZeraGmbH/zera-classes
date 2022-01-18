#ifndef SOURCEDEVICESUBJECT_H
#define SOURCEDEVICESUBJECT_H

#include <QObject>

class IoWorkerCmdPack;
class SourceDeviceObserver;

class SourceDeviceSubject : public QObject
{
    Q_OBJECT
public:
    void notifyObservers(const IoWorkerCmdPack response);
signals:
    void sigResponseReceived(const IoWorkerCmdPack response);
};

#endif // SOURCEDEVICESUBJECT_H

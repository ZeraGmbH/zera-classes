#ifndef SOURCEDEVICESUBJECT_H
#define SOURCEDEVICESUBJECT_H

#include <QObject>

class SourceWorkerCmdPack;
class SourceDeviceObserver;

class SourceDeviceSubject : public QObject
{
    Q_OBJECT
public:
    void notifyObservers(const SourceWorkerCmdPack response);
signals:
    void sigResponseReceived(const SourceWorkerCmdPack response);
};

#endif // SOURCEDEVICESUBJECT_H

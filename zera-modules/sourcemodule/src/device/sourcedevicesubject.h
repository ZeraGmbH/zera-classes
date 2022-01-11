#ifndef SOURCEDEVICESUBJECT_H
#define SOURCEDEVICESUBJECT_H

#include <QList>

class SourceWorkerCmdPack;
class SourceDeviceObserver;

class SourceDeviceSubject
{
public:
    void attach(SourceDeviceObserver* observer);
    void notifyObservers(const SourceWorkerCmdPack &response);

private:
    QList<SourceDeviceObserver*> m_observers;
};

#endif // SOURCEDEVICESUBJECT_H

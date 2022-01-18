#ifndef SOURCEDEVICESUBJECT_H
#define SOURCEDEVICESUBJECT_H

#include <QObject>

class IoTransferDataGroup;
class SourceDeviceObserver;

class SourceDeviceSubject : public QObject
{
    Q_OBJECT
public:
    void notifyObservers(const IoTransferDataGroup response);
signals:
    void sigResponseReceived(const IoTransferDataGroup response);
};

#endif // SOURCEDEVICESUBJECT_H

#ifndef SOURCEDEVICESUBJECT_H
#define SOURCEDEVICESUBJECT_H

#include <QObject>

class IoMultipleTransferGroup;
class SourceDeviceObserver;

class SourceDeviceSubject : public QObject
{
    Q_OBJECT
public:
    void notifyObservers(const IoMultipleTransferGroup response);
signals:
    void sigResponseReceived(const IoMultipleTransferGroup response);
};

#endif // SOURCEDEVICESUBJECT_H

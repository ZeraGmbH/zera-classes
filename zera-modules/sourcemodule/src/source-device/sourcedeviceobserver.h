#ifndef SOURCEDEVICEOBSERVER_H
#define SOURCEDEVICEOBSERVER_H

#include <QObject>

class IoTransferDataGroup;
class SourceDeviceSubject;


class SourceDeviceObserver : public QObject
{
    Q_OBJECT
public:
    SourceDeviceObserver(SourceDeviceSubject* subject);

protected slots:
    virtual void updateResponse(IoTransferDataGroup transferGroup) = 0;
    friend class SourceDeviceSubject;
};

#endif // SOURCEDEVICEOBSERVER_H

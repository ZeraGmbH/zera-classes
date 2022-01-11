#ifndef SOURCEDEVICEOBSERVER_H
#define SOURCEDEVICEOBSERVER_H

class SourceWorkerCmdPack;
class SourceDeviceSubject;

class SourceDeviceObserver
{
public:
    SourceDeviceObserver(SourceDeviceSubject* subject);

protected:
    friend class SourceDeviceSubject;
    virtual void updateResponse(const SourceWorkerCmdPack& cmdPack) = 0;

    SourceDeviceSubject* m_subject;
};

#endif // SOURCEDEVICEOBSERVER_H

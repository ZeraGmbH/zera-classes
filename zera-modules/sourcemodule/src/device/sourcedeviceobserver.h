#ifndef SOURCEDEVICEOBSERVER_H
#define SOURCEDEVICEOBSERVER_H

class SourceWorkerCmdPack;
class SourceDeviceSubject;

class SourceDeviceObserver
{
public:
    SourceDeviceObserver(SourceDeviceSubject* subject);
    virtual void updateResponse(const SourceWorkerCmdPack& cmdPack) = 0;
protected:
    SourceDeviceSubject* m_subject;
};

#endif // SOURCEDEVICEOBSERVER_H

#include "sourcedevicesubject.h"
#include "sourcedeviceobserver.h"
#include "io-worker/ioworker.h"

void SourceDeviceSubject::notifyObservers(const IoWorkerCmdPack response)
{
    emit sigResponseReceived(response);
}

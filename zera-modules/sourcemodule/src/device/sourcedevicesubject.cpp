#include "sourcedevicesubject.h"
#include "sourcedeviceobserver.h"
#include "io-interface/sourceioworker.h"

void SourceDeviceSubject::notifyObservers(const SourceWorkerCmdPack response)
{
    emit sigResponseReceived(response);
}

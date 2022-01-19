#include "sourcedevicesubject.h"
#include "sourcedeviceobserver.h"
#include "io-queue/ioqueue.h"

void SourceDeviceSubject::notifyObservers(const IoTransferDataGroup response)
{
    emit sigResponseReceived(response);
}

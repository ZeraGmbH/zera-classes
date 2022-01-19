#include "sourcedeviceobserver.h"
#include "sourcedevicesubject.h"
#include "io-queue/ioqueue.h"

SourceDeviceObserver::SourceDeviceObserver(SourceDeviceSubject *subject)
{
    connect(subject, &SourceDeviceSubject::sigResponseReceived,
            this, &SourceDeviceObserver::updateResponse);
}

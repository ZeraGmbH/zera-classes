#include "sourcedeviceobserver.h"
#include "sourcedevicesubject.h"
#include "io-worker/ioworker.h"

SourceDeviceObserver::SourceDeviceObserver(SourceDeviceSubject *subject)
{
    connect(subject, &SourceDeviceSubject::sigResponseReceived,
            this, &SourceDeviceObserver::updateResponse);
}

#include "sourcedeviceobserver.h"
#include "sourcedevicesubject.h"
#include "io-interface/ioworker.h"

SourceDeviceObserver::SourceDeviceObserver(SourceDeviceSubject *subject)
{
    connect(subject, &SourceDeviceSubject::sigResponseReceived,
            this, &SourceDeviceObserver::updateResponse);
}

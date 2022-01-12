#include "sourcedeviceobserver.h"
#include "sourcedevicesubject.h"
#include "io-interface/sourceioworker.h"

SourceDeviceObserver::SourceDeviceObserver(SourceDeviceSubject *subject)
{
    connect(subject, &SourceDeviceSubject::sigResponseReceived,
            this, &SourceDeviceObserver::updateResponse);
}

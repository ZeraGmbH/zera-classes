#include "sourcedeviceobserver.h"
#include "sourcedevicesubject.h"

SourceDeviceObserver::SourceDeviceObserver(SourceDeviceSubject *subject)
{
    m_subject = subject;
    subject->attach(this);
}

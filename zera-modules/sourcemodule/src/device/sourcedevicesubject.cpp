#include "sourcedevicesubject.h"
#include "sourcedeviceobserver.h"

void SourceDeviceSubject::attach(SourceDeviceObserver *observer)
{
    m_observers.append(observer);
}

void SourceDeviceSubject::notifyObservers(const SourceWorkerCmdPack &response)
{
    for(auto observer: m_observers) {
        observer->updateResponse(response);
    }
}

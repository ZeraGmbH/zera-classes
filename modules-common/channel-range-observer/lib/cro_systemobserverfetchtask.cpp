#include "cro_systemobserverfetchtask.h"

namespace ChannelRangeObserver {

SystemObserverFetchTaskPtr SystemObserverFetchTask::create(SystemObserverPtr systemObserver)
{
    return std::make_unique<SystemObserverFetchTask>(systemObserver);
}

SystemObserverFetchTask::SystemObserverFetchTask(SystemObserverPtr systemObserver) :
    m_systemObserver(systemObserver)
{
}

void SystemObserverFetchTask::start()
{
    connect(m_systemObserver.get(), &SystemObserver::sigFullScanFinished,
            this, &SystemObserverFetchTask::finishTask);
    m_systemObserver->startFullScan();
}

}

#ifndef SYSTEMOBSERVERFETCHTASK_H
#define SYSTEMOBSERVERFETCHTASK_H

#include "cro_systemobserver.h"
#include <tasktemplate.h>

namespace ChannelRangeObserver {

class SystemObserverFetchTask;
typedef std::unique_ptr<SystemObserverFetchTask> SystemObserverFetchTaskPtr;

class SystemObserverFetchTask : public TaskTemplate
{
    Q_OBJECT
public:
    static SystemObserverFetchTaskPtr create(SystemObserverPtr systemObserver);
    SystemObserverFetchTask(SystemObserverPtr systemObserver);
    void start() override;
private:
    SystemObserverPtr m_systemObserver;
};

}
#endif // SYSTEMOBSERVERFETCHTASK_H

#include "sourcestateperiodicpollerfortest.h"

SourceStatePeriodicPollerForTest::SourceStatePeriodicPollerForTest(SourceTransactionStartNotifier::Ptr sourceIoWithNotificationForQuery, int pollTime) :
    SourceStatePeriodicPoller(sourceIoWithNotificationForQuery, pollTime)
{

}

int SourceStatePeriodicPollerForTest::isPeriodicPollActive() const
{
    return m_pollTimer->isRunning();
}

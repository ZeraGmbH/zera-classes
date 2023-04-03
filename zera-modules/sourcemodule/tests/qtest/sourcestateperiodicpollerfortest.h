#ifndef SOURCESTATEPERIODICPOLLERFORTEST_H
#define SOURCESTATEPERIODICPOLLERFORTEST_H

#include "sourceperiodicpollerstate.h"

class SourceStatePeriodicPollerForTest : public SourceStatePeriodicPoller
{
public:
    typedef QSharedPointer<SourceStatePeriodicPollerForTest> Ptr;
    SourceStatePeriodicPollerForTest(SourceTransactionStartNotifier::Ptr sourceIoWithNotificationForQuery,
                                     int pollTime = 500);
    int isPeriodicPollActive() const;
};

#endif // SOURCESTATEPERIODICPOLLERFORTEST_H

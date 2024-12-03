#ifndef CRO_CHANNEL_OBSERVER_RESETTER_H
#define CRO_CHANNEL_OBSERVER_RESETTER_H

#include "cro_systemobserver.h"

namespace ChannelRangeObserver {

class SystemObserverResetter
{
public:
    static void clear(SystemObserver* observer);
};

}
#endif // CRO_CHANNEL_OBSERVER_RESETTER_H

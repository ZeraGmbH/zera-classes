#ifndef AllCHANNELOBSERVERRESETTER_H
#define AllCHANNELOBSERVERRESETTER_H

#include "allchannelsobserver.h"

namespace ChannelRangeObserver {

class AllChannelsObserverResetter
{
public:
    static void clear(AllChannelsObserver* observer);
};

}
#endif // AllCHANNELOBSERVERRESETTER_H

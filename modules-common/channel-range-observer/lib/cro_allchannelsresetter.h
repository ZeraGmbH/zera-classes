#ifndef CRO_All_CHANNELS_RESETTER_H
#define CRO_All_CHANNELS_RESETTER_H

#include "cro_allchannels.h"

namespace ChannelRangeObserver {

class AllChannelsResetter
{
public:
    static void clear(AllChannels* observer);
};

}
#endif // CRO_All_CHANNELS_RESETTER_H

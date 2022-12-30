#ifndef CHANNELCOLLECTIONREQUESTER_H
#define CHANNELCOLLECTIONREQUESTER_H

#include "channelcollection.h"
#include <QObject>

class ChannelCollectionRequester : public QObject
{
    Q_OBJECT
public:
    explicit ChannelCollectionRequester();
    void request();
    ChannelCollectionPtr getChannels();
signals:
    void available();
};

#endif // CHANNELCOLLECTIONREQUESTER_H

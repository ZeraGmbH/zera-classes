#ifndef CHANNELCOLLECTION_H
#define CHANNELCOLLECTION_H

#include <pcbinterface.h>
#include "measchannel.h"
#include <memory>

class ChannelCollection : public QObject
{
    Q_OBJECT
public:
    ChannelCollection(Zera::Server::PcbInterfacePtr pcbInterace);
    MeasChannel* getChannelBySysName(QString name);
    MeasChannel* getChannelByAlias(QString alias);
};

typedef std::shared_ptr<ChannelCollection> ChannelCollectionPtr;

#endif // CHANNELCOLLECTION_H

#ifndef CHANNELINFOSINGLETON_H
#define CHANNELINFOSINGLETON_H

#include "channelinfo.h"

class ChannelInfoManager : public QObject
{
    Q_OBJECT
public:
    ChannelInfoManager *getInstance();
    void load();
    int getChannelCount() const;
    const ChannelInfo *getChannelInfo(int channelNo) const;
signals:
    void sigLoaded();
    void sigChannelChanged(int channel);
private:
    ChannelInfoManager();
    static ChannelInfoManager *m_instance;

};

#endif // CHANNELINFOSINGLETON_H

#include "channelinfomanager.h"

ChannelInfoManager *ChannelInfoManager::m_instance = nullptr;

ChannelInfoManager *ChannelInfoManager::getInstance()
{
    if(!m_instance)
        m_instance = new ChannelInfoManager;
    return m_instance;
}

ChannelInfoManager::ChannelInfoManager()
{
}

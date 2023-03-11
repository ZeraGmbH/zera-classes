#include "resourceclient.h"
#include "resourcebookkeeper.h"

ResourceClient::ResourceClient(ResourceBookkeeper *bookKeeper, secret) :
    m_bookKeeper(bookKeeper)
{
}

ResourceClient::~ResourceClient()
{
    if(m_bookKeeper)
        m_bookKeeper->clientAboutToDie(this);
}

int ResourceClient::getAllocated()
{
    return m_allocated;
}

bool ResourceClient::alloc(int amount)
{
    if(!m_bookKeeper || !m_bookKeeper->clientAlloc(amount))
        return false;
    m_allocated += amount;
    return true;
}

void ResourceClient::freeAll()
{
    if(m_bookKeeper)
        m_bookKeeper->clientFree(m_allocated);
    m_allocated = 0;
}

void ResourceClient::keeperAboutToDie()
{
    m_bookKeeper = nullptr;
    freeAll();
}

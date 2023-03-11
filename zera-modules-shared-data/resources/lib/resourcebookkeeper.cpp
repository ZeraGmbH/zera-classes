#include "resourcebookkeeper.h"
#include "resourceclient.h"

ResourceBookkeeper::ResourceBookkeeper(int totalAvail) :
    m_total(totalAvail)
{
}

ResourceBookkeeper::~ResourceBookkeeper()
{
    for(const auto &client : qAsConst(m_clients))
        client->keeperAboutToDie();
}

int ResourceBookkeeper::getTotal() const
{
    return m_total;
}

int ResourceBookkeeper::getAllocated() const
{
    return m_allocated;
}

int ResourceBookkeeper::getFree() const
{
    return getTotal() - getAllocated();
}

int ResourceBookkeeper::getClientCount() const
{
    return m_clients.count();
}

ResourceClientPtr ResourceBookkeeper::registerClient()
{
    ResourceClientPtr client = std::make_unique<ResourceClient>(this, ResourceClient::secret());
    m_clients.insert(client.get());
    return client;
}

bool ResourceBookkeeper::clientAlloc(int amount)
{
    if(amount <= 0 || amount > getFree())
        return false;
    m_allocated += amount;
    emit sigAllocationChanged();
    return true;
}

void ResourceBookkeeper::clientFree(int amount)
{
    m_allocated -= amount;
    if(amount)
        emit sigAllocationChanged();
}

void ResourceBookkeeper::clientAboutToDie(ResourceClient *client)
{
    m_clients.remove(client);
    clientFree(client->getAllocated());
}

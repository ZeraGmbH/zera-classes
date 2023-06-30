#ifndef RESOURCEBOOKKEEPER_H
#define RESOURCEBOOKKEEPER_H

#include <QObject>
#include <QSet>
#include <memory>

class ResourceClient;

typedef std::unique_ptr<ResourceClient> ResourceClientPtr;

class ResourceBookkeeper : public QObject
{
    Q_OBJECT
public:
    ResourceBookkeeper(int totalAvail);
    virtual ~ResourceBookkeeper();
    ResourceClientPtr registerClient();
    int getTotal() const;
    int getAllocated() const;
    int getFree() const;
    int getClientCount() const;
signals:
    void sigAllocationChanged();
private:
    friend ResourceClient;
    bool clientAlloc(int amount);
    void clientFree(int amount);
    void clientAboutToDie(ResourceClient* client);
    const int m_total;
    int m_allocated = 0;
    QSet<ResourceClient*> m_clients;
};


#endif // RESOURCEBOOKKEEPER_H

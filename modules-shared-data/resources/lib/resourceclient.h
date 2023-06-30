#ifndef RESOURCECLIENT_H
#define RESOURCECLIENT_H

#include <memory>

class ResourceBookkeeper;

class ResourceClient
{
private:
    // Just our friend ResourceBookkeeper can create us (by make_unique) - see
    // https://devblogs.microsoft.com/oldnewthing/20220721-00/?p=106879
    struct secret { explicit secret() = default; };
public:
    ResourceClient(ResourceBookkeeper* bookKeeper, secret);
    ~ResourceClient();
    int getAllocated();
    bool alloc(int amount);
    void freeAll();
private:
    friend ResourceBookkeeper;
    void keeperAboutToDie();
    ResourceBookkeeper* m_bookKeeper;
    int m_allocated = 0;
};

#endif // RESOURCECLIENT_H

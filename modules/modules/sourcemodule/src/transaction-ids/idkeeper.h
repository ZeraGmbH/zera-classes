#ifndef SOURCEIDKEEPER_H
#define SOURCEIDKEEPER_H

#include <QSet>

class IIdKeeper
{
public:
    virtual void setPending(int id) = 0;
    virtual bool isPendingAndRemoveIf(int id) = 0;
    virtual bool hasPending() const = 0;
    virtual void clear() = 0;
};

class IdKeeperSingle : public IIdKeeper
{
public:
    void setPending(int id) override;
    int getPending() const;

    bool isPendingAndRemoveIf(int id) override;

    bool hasPending() const override;
    void clear() override;
private:
    bool m_isCurrPending = false;
    int m_currentId;
};

class IdKeeperMulti : public IIdKeeper
{
public:
    void setPending(int id) override;

    bool isPendingAndRemoveIf(int id) override;

    bool hasPending() const override;
    void clear() override;
private:
    QSet<int> m_pendingIds;
};

#endif // SOURCEIDKEEPER_H

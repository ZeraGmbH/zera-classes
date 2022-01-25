#include "idkeeper.h"

void IdKeeperSingle::setPending(int id)
{
    m_currentId = id;
    m_isCurrPending = true;
}

int IdKeeperSingle::getPending() const
{
    return m_currentId;
}

bool IdKeeperSingle::isPendingAndRemoveIf(int id)
{
    if(m_isCurrPending && id == m_currentId) {
        m_isCurrPending = false;
        return true;
    }
    return false;
}

bool IdKeeperSingle::hasPending() const
{
    return m_isCurrPending;
}

void IdKeeperSingle::clear()
{
    m_isCurrPending = false;
}



void IdKeeperMulti::setPending(int id)
{
    m_pendingIds.insert(id);
}

bool IdKeeperMulti::isPendingAndRemoveIf(int id)
{
    return m_pendingIds.remove(id);
}

bool IdKeeperMulti::hasPending() const
{
    return !m_pendingIds.isEmpty();
}

void IdKeeperMulti::clear()
{
    m_pendingIds.clear();
}

#include "sourceidkeeper.h"

SourceIdKeeper::SourceIdKeeper()
{

}

void SourceIdKeeper::setCurrent(int id)
{
    m_currentId = id;
    m_isActive = true;
}

int SourceIdKeeper::getCurrent()
{
    return m_currentId;
}

bool SourceIdKeeper::isCurrAndDeactivateIf(int id)
{
    if(m_isActive && id == m_currentId) {
        m_isActive = false;
        return true;
    }
    return false;
}

bool SourceIdKeeper::isActive()
{
    return m_isActive;
}

void SourceIdKeeper::deactivate()
{
    m_isActive = false;
}

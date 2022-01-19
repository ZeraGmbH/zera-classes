#include "ioidkeeper.h"

IoIdKeeper::IoIdKeeper()
{
}

void IoIdKeeper::setCurrent(int id)
{
    m_currentId = id;
    m_isActive = true;
}

int IoIdKeeper::getCurrent()
{
    return m_currentId;
}

bool IoIdKeeper::isCurrAndDeactivateIf(int id)
{
    if(m_isActive && id == m_currentId) {
        m_isActive = false;
        return true;
    }
    return false;
}

bool IoIdKeeper::isActive() const
{
    return m_isActive;
}

void IoIdKeeper::deactivate()
{
    m_isActive = false;
}

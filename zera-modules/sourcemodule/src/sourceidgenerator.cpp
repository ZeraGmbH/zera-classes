#include "sourceidgenerator.h"

int cSourceIdGenerator::nextID()
{
    if(++m_currentID == 0) {
        ++m_currentID;
    }
    return m_currentID;
}

#include "ioidgenerator.h"

int IoIdGenerator::m_currentIDStatic = -1;

int IoIdGenerator::nextID()
{
    return ++m_currentID;
}

int IoIdGenerator::nextIDStatic()
{
    return ++m_currentIDStatic;
}

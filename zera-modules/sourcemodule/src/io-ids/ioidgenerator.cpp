#include "ioidgenerator.h"

int IoIdGenerator::nextID()
{
    return ++m_currentID;
}


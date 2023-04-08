#include "idgenerator.h"

int IoIdGenerator::nextID()
{
    return ++m_currentID;
}


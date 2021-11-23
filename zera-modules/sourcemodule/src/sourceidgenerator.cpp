#include "sourceidgenerator.h"

int cSourceIdGenerator::nextID()
{
    return ++m_currentID;
}

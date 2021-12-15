#include "sourceidgenerator.h"

int SourceIdGenerator::nextID()
{
    return ++m_currentID;
}

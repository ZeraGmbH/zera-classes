#include "sourcetransactionidgenerator.h"

int cSourceTransactionIdGenerator::nextTransactionID()
{
    if(++m_currentTransActionID == 0) {
        ++m_currentTransActionID;
    }
    return m_currentTransActionID;
}

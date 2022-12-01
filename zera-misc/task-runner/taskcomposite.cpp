#include "taskcomposit.h"

int TaskComposite::m_currTaskId = 0;

int TaskComposite::getNextTaskId()
{
    return ++m_currTaskId;
}

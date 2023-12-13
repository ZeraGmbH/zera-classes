#include "modulegroupnumerator.h"

ModuleGroupNumerator::ModuleGroupNumerator()
{
}

int ModuleGroupNumerator::requestModuleNum(int moduleNumLargerThan0)
{
    int nextModuleNo = m_requestedModuleNums.count() + 1;
    if(!m_requestedModuleNums.contains(nextModuleNo)) {
        m_requestedModuleNums.insert(nextModuleNo);
        return nextModuleNo;
    }
    return -1;
}

void ModuleGroupNumerator::freeModuleNum(int moduleNum)
{
    m_requestedModuleNums.remove(moduleNum);
}

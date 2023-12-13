#include "modulegroupnumerator.h"

int ModuleGroupNumerator::requestModuleNum(int moduleNumRequested)
{
    if(moduleNumRequested > 0)
        return tryAddNewNum(moduleNumRequested);
    for(int newNum = 1; ; newNum++) {
        if(!m_requestedModuleNums.contains(newNum))
            return tryAddNewNum(newNum);
    }
    return tryAddNewNum(m_requestedModuleNums.count() + 1);
}

void ModuleGroupNumerator::freeModuleNum(int moduleNum)
{
    m_requestedModuleNums.remove(moduleNum);
}

int ModuleGroupNumerator::tryAddNewNum(int newNum)
{
    if(!m_requestedModuleNums.contains(newNum)) {
        m_requestedModuleNums.insert(newNum);
        return newNum;
    }
    return -1;
}

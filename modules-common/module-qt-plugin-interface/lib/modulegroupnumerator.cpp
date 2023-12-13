#include "modulegroupnumerator.h"

ModuleGroupNumerator::ModuleGroupNumerator()
{
}

int ModuleGroupNumerator::requestModuleNum(int moduleNumLargerThan0)
{
    if(!m_requestedModuleNums.contains(moduleNumLargerThan0)) {
        m_requestedModuleNums.insert(moduleNumLargerThan0);
        return moduleNumLargerThan0;
    }
    return -1;
}

void ModuleGroupNumerator::freeModuleNum(int moduleNum)
{
    m_requestedModuleNums.remove(moduleNum);
}

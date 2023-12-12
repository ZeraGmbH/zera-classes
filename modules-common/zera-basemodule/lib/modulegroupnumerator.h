#ifndef MODULEGROUPNUMERATOR_H
#define MODULEGROUPNUMERATOR_H

#include <QSet>

class ModuleGroupNumerator
{
public:
    ModuleGroupNumerator();
    int requestModuleNum(int moduleNumLargerThan0);
    void freeModuleNum(int moduleNum);
private:
    QSet<int> m_requestedModuleNums;
};

#endif // MODULEGROUPNUMERATOR_H

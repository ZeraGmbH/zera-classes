#ifndef MODULEGROUPNUMERATOR_H
#define MODULEGROUPNUMERATOR_H

#include <QSet>
#include <memory>

class ModuleGroupNumerator
{
public:
    ModuleGroupNumerator();
    int requestModuleNum(int moduleNumLargerThan0);
    void freeModuleNum(int moduleNum);
private:
    QSet<int> m_requestedModuleNums;
};

typedef std::unique_ptr<ModuleGroupNumerator> ModuleGroupNumeratorPtr;

#endif // MODULEGROUPNUMERATOR_H

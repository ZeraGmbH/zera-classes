#ifndef MODULEGROUPNUMERATOR_H
#define MODULEGROUPNUMERATOR_H

#include <QSet>
#include <memory>

class ModuleGroupNumerator
{
public:
    static constexpr int NoNumRequest = 0;
    int requestModuleNum(int moduleNumRequested);
    void freeModuleNum(int moduleNum);
private:
    int tryAddNewNum(int newNum);
    QSet<int> m_requestedModuleNums;
};

typedef std::unique_ptr<ModuleGroupNumerator> ModuleGroupNumeratorPtr;

#endif // MODULEGROUPNUMERATOR_H

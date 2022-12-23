#ifndef SCPICOMMANDCHECKER_H
#define SCPICOMMANDCHECKER_H

#include <scpi.h>
#include <scpiobject.h>
#include <QStringList>

class ScpiCommandChecker
{
public:
    ScpiCommandChecker(QStringList parentNodeNames, QString &nodeName,
                       quint8 scpiType);
    bool matches(QString cmd);
private:
    cSCPI m_scpiTree;
};

#endif // SCPICOMMANDCHECKER_H

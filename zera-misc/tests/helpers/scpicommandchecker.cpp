#include "scpicommandchecker.h"

ScpiCommandChecker::ScpiCommandChecker(QStringList parentNodeNames, QString &nodeName,
                                       quint8 scpiType) :
    m_scpiTree("TestScpi")
{
}

bool ScpiCommandChecker::matches(QString cmd)
{

}

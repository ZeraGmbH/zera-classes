#include "testdspinterface.h"

TestDspInterface::TestDspInterface(QStringList valueNamesList) :
    m_valueNamesList(valueNamesList)
{
}

QStringList TestDspInterface::getValueList()
{
    return m_valueNamesList;
}

void TestDspInterface::addCycListItem(QString cmd)
{
    MockDspInterface::addCycListItem(cmd);
    m_cyclicListItems.append(cmd);
}

void TestDspInterface::addCycListItems(const QStringList &cmds)
{
    MockDspInterface::addCycListItems(cmds);
    m_cyclicListItems.append(cmds);
}

QByteArray TestDspInterface::dumpCycListItem()
{
    if(m_cyclicListItems.count() == 0)
        return "";
    return m_cyclicListItems.join("\n").toLatin1() + "\n"; // Qt-Creator appends a final linefeed automatically
}

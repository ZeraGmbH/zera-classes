#include "testdspinterface.h"

TestDspInterface::TestDspInterface(QStringList valueNamesList) :
    m_valueNamesList(valueNamesList)
{
}

QStringList TestDspInterface::getValueList()
{
    return m_valueNamesList;
}

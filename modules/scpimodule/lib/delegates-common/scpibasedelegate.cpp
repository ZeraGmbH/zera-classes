#include "scpibasedelegate.h"
#include <scpi.h>

namespace SCPIMODULE {

int ScpiBaseDelegate::m_instanceCount = 0;

ScpiBaseDelegate::ScpiBaseDelegate() :
    ScpiObject("", 0)
{
    m_instanceCount++;
}

ScpiBaseDelegate::ScpiBaseDelegate(const QString &cmdParent, const QString &cmd, quint8 scpiCmdQueryFlags) :
    ScpiObject(cmd, scpiCmdQueryFlags),
    m_cmdParent(cmdParent.split(":", Qt::SkipEmptyParts))
{
    m_instanceCount++;
}

ScpiBaseDelegate::~ScpiBaseDelegate()
{
    m_instanceCount--;
}

const QStringList &ScpiBaseDelegate::getCmdParent() const
{
    return m_cmdParent;
}

int ScpiBaseDelegate::getInstanceCount()
{
    return m_instanceCount;
}

}

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
    m_sCmdParent(cmdParent)
{
    m_instanceCount++;
}

ScpiBaseDelegate::~ScpiBaseDelegate()
{
    m_instanceCount--;
}

const QString &ScpiBaseDelegate::getCmdParent() const
{
    return m_sCmdParent;
}

int ScpiBaseDelegate::getInstanceCount()
{
    return m_instanceCount;
}

}

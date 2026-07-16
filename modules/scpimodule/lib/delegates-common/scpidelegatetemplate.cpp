#include "scpidelegatetemplate.h"

namespace SCPIMODULE {

int ScpiDelegateTemplate::m_instanceCount = 0;

ScpiDelegateTemplate::ScpiDelegateTemplate() :
    ScpiObject("", 0)
{
    m_instanceCount++;
}

ScpiDelegateTemplate::ScpiDelegateTemplate(const QString &cmdParent, const QString &cmd, quint8 scpiQueryCmdFlags) :
    ScpiObject(cmd, scpiQueryCmdFlags),
    m_cmdParent(cmdParent.split(":", Qt::SkipEmptyParts))
{
    m_instanceCount++;
}

ScpiDelegateTemplate::~ScpiDelegateTemplate()
{
    m_instanceCount--;
}

const QStringList &ScpiDelegateTemplate::getCmdParent() const
{
    return m_cmdParent;
}

int ScpiDelegateTemplate::getInstanceCount()
{
    return m_instanceCount;
}

}

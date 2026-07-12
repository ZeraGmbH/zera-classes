#include "scpidelegatecatalog.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>
#include <vcmp_componentdata.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>

namespace SCPIMODULE {

ScpiDelegateCatalog::ScpiDelegateCatalog(const QString &cmdParent,
                                         const QString &cmd,
                                         quint8 scpiCmdQueryFlags,
                                         cSCPIModule *scpimodule,
                                         cSCPICmdInfoPtr scpicmdinfo) :
    ScpiDelegateTemplate(cmdParent, cmd, scpiCmdQueryFlags),
    m_pModule(scpimodule),
    m_pSCPICmdInfo(scpicmdinfo)
{
    setOutput(m_pSCPICmdInfo);
}

void ScpiDelegateCatalog::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = scpi;
    if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0))// test if we got an allowed query
        client->handleCmdFinish(m_sAnswer, scpiTransactionId);
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiDelegateCatalog::setOutput(const QVariant &modInterface)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(modInterface.toByteArray());
    QJsonObject jsonObj = jsonDoc.object();
    jsonObj = jsonObj["ComponentInfo"].toObject();
    jsonObj = jsonObj[m_pSCPICmdInfo->componentOrRpcName].toObject();
    jsonObj = jsonObj["Validation"].toObject();
    QJsonArray jsonArr;
    jsonArr = jsonObj["Data"].toArray();

    const QVariantList vaList = jsonArr.toVariantList();
    m_sAnswer.clear();
    for(int i=0; i<vaList.count(); ++i) {
        if(i>0)
            m_sAnswer += ";";
        m_sAnswer += vaList.at(i).toString();
    }
}

void ScpiDelegateCatalog::setOutput(cSCPICmdInfoPtr scpicmdinfo)
{
    QVariant ModInterface = m_pModule->getStorageDb()->getStoredValue(scpicmdinfo->entityId, QString("INF_ModuleInterface"));
    setOutput(ModInterface);
}


}

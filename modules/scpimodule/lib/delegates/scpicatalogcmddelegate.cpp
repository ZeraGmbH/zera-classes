#include "scpicatalogcmddelegate.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>
#include <vcmp_componentdata.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>

namespace SCPIMODULE {

cSCPICatalogCmdDelegate::cSCPICatalogCmdDelegate(const QString &cmdParent,
                                                 const QString &cmd,
                                                 quint8 type,
                                                 cSCPIModule *scpimodule,
                                                 cSCPICmdInfoPtr scpicmdinfo) :
    ScpiBaseDelegate(cmdParent, cmd, type), m_pModule(scpimodule), m_pSCPICmdInfo(scpicmdinfo)
{
    setOutput(m_pSCPICmdInfo);
}

void cSCPICatalogCmdDelegate::executeSCPI(cSCPIClient *client, const QString &scpi)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = scpi;
    if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0))// test if we got an allowed query
        client->receiveAnswer(m_sAnswer);
    else
        client->receiveStatus(ZSCPI::nak);
}

void cSCPICatalogCmdDelegate::setOutput(const QVariant &modInterface)
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

void cSCPICatalogCmdDelegate::setOutput(cSCPICmdInfoPtr scpicmdinfo)
{
    QVariant ModInterface = m_pModule->getStorageDb()->getStoredValue(scpicmdinfo->entityId, QString("INF_ModuleInterface"));
    setOutput(ModInterface);
}


}

#include "scpicatalogcmddelegate.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>
#include <vcmp_componentdata.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantList>

namespace SCPIMODULE {

cSCPICatalogCmdDelegate::cSCPICatalogCmdDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule *scpimodule,  cSCPICmdInfo* scpicmdinfo)
    :ScpiBaseDelegate(cmdParent, cmd, type), m_pModule(scpimodule), m_pSCPICmdInfo(scpicmdinfo)
{
    setOutput(m_pSCPICmdInfo);
}

cSCPICatalogCmdDelegate::~cSCPICatalogCmdDelegate()
{
    delete m_pSCPICmdInfo;
}

void cSCPICatalogCmdDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = sInput;
    if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0))// test if we got an allowed query
        client->receiveAnswer(m_sAnswer);
    else
        client->receiveStatus(ZSCPI::nak);
}

void cSCPICatalogCmdDelegate::setOutput(cSCPICmdInfo *scpicmdinfo)
{
    QVariant ModInterface = m_pModule->getStorageSystem()->getDb()->getStoredValue(scpicmdinfo->entityId, QString("INF_ModuleInterface"));
    setOutput(ModInterface);
}

void cSCPICatalogCmdDelegate::setOutput(QVariant modInterface)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(modInterface.toByteArray());
    QJsonObject jsonObj = jsonDoc.object();
    jsonObj = jsonObj["ComponentInfo"].toObject();
    jsonObj = jsonObj[m_pSCPICmdInfo->componentName].toObject();
    jsonObj = jsonObj["Validation"].toObject();
    QJsonArray jsonArr;
    jsonArr = jsonObj["Data"].toArray();

    QVariantList vaList = jsonArr.toVariantList();
    m_sAnswer = vaList.at(0).toString();
    int n = vaList.count();
    if (n > 1) {
        for (int i = 1; i < vaList.count(); i++)
            m_sAnswer = m_sAnswer +";" + vaList.at(i).toString();
    }
}

}





#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <veinhub.h>
#include <veinpeer.h>
#include <veinentity.h>
#include <scpi.h>

#include "scpimeasure.h"
#include "scpiinterface.h"
#include "moduleinterface.h"
#include "scpicmdinfo.h"
#include "scpiparameterdelegate.h"
#include "scpimeasuredelegate.h"


namespace SCPIMODULE
{

cModuleInterface::cModuleInterface(VeinPeer *peer, cSCPIInterface *iface)
    :cBaseInterface(peer, iface)
{
}


cModuleInterface::~cModuleInterface()
{
    for (int i = 0; i < m_scpiParameterDelegateList.count(); i++ )
        delete m_scpiParameterDelegateList.at(i);
    for (int i = 0; i < m_scpiMeasureDelegateHash.count(); i++ )
        delete m_scpiParameterDelegateList.at(i);
}


bool cModuleInterface::setupInterface()
{
    cSCPICmdInfo scpiCmdInfo;
    bool noError;

    noError = true;
    QList<VeinPeer *> peerList;
    VeinHub *hub = m_pPeer->getHub();
    peerList = hub->listPeers();

    for (int i = 0; i < peerList.count(); i++)
    {
        QJsonDocument jsonDoc; // we parse over all moduleinterface entities
        VeinEntity* entity = peerList.at(i)->getEntityByName(QString("INF_ModuleInterface"));

        if (entity != 0) // modulemangers and interfaces do not export INF_ModuleInterface
        {
            jsonDoc = QJsonDocument::fromBinaryData(entity->getValue().toByteArray());

            if ( !jsonDoc.isNull() && jsonDoc.isObject() )
            {
                VeinPeer* peer;
                QJsonObject jsonObj;
#ifdef DEBUG
                qDebug() << jsonDoc;
#endif
                jsonObj = jsonDoc.object();
                scpiCmdInfo.scpiModuleName = jsonObj["SCPIModuleName"].toString();
                peer = hub->getPeerByName(jsonObj["VeinPeer"].toString());
                scpiCmdInfo.peer = peer;

                QJsonArray jsonEntityArr = jsonObj["Entities"].toArray();

                for (int j = 0; j < jsonEntityArr.count(); j++)
                {
                    QJsonObject jsonEntityObj;
                    jsonEntityObj = jsonEntityArr[j].toObject();
                    QJsonValue jsonVal;
                    jsonVal = jsonEntityObj["Name"];
                    QString s = jsonVal.toString();
                    scpiCmdInfo.entity = peer->getEntityByName(s);
                    //scpiCmdInfo.entity = peer->getEntityByName(jsonEntityObj["Name"].toString());
                    scpiCmdInfo.scpiModel = jsonEntityObj["SCPI"].toArray()[0].toString();
                    scpiCmdInfo.cmdNode = jsonEntityObj["SCPI"].toArray()[1].toString();
                    scpiCmdInfo.type = jsonEntityObj["SCPI"].toArray()[2].toString();
                    scpiCmdInfo.unit = jsonEntityObj["SCPI"].toArray()[3].toString();

                    if (scpiCmdInfo.type != "0") // we have to add this entity to our interface
                        addSCPICommand(scpiCmdInfo);
                }
            }
            else
            {
                //noError = false;
                //break;
            }
        }
    }

    return noError;
}


void cModuleInterface::addSCPICommand(cSCPICmdInfo &scpiCmdInfo)
{
    if (scpiCmdInfo.scpiModel == "MEASURE")

    {
        // in case of measure model we have to add several commands for each value

        cSCPIMeasure* measureObject = new cSCPIMeasure(scpiCmdInfo.entity, scpiCmdInfo.scpiModuleName, scpiCmdInfo.cmdNode, scpiCmdInfo.unit);

        addSCPIMeasureCommand(QString(""), QString("MEASURE"), SCPI::isNode | SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("CONFIGURE"), SCPI::isNode | SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("READ"), SCPI::isNode | SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString(""), QString("INIT"), SCPI::isNode | SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString(""), QString("FETCH"), SCPI::isNode | SCPI::isQuery, SCPIModelType::fetch, measureObject);

        addSCPIMeasureCommand(QString("MEASURE"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::fetch, measureObject);

        addSCPIMeasureCommand(QString("MEASURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, SCPIModelType::fetch, measureObject);

    }
    else
    {
        bool ok;
        cSCPIParameterDelegate* delegate;

        QString cmdComplete;
        cmdComplete = QString("%1:%2:3").arg(scpiCmdInfo.scpiModel).arg(scpiCmdInfo.scpiModuleName).arg(scpiCmdInfo.cmdNode);
        QStringList nodeNames = cmdComplete.split(':');
        QString cmdNode = nodeNames.takeLast();
        QString cmdParent = nodeNames.join(':');

        /*
        QString cmdParent;
        if (scpiCmdInfo.addParents == "")
            cmdParent = QString("%1:%2").arg(scpiCmdInfo.scpiModel).arg(scpiCmdInfo.scpiModuleName);
        else
            cmdParent = QString("%1:%2:%3").arg(scpiCmdInfo.scpiModel).arg(scpiCmdInfo.scpiModuleName).arg(scpiCmdInfo.addParents);
        */

        delegate = new cSCPIParameterDelegate(cmdParent, cmdNode, scpiCmdInfo.type.toInt(&ok), scpiCmdInfo.peer, scpiCmdInfo.entity);
        m_scpiParameterDelegateList.append(delegate);
        m_pSCPIInterface->addSCPICommand(delegate);
    }
}


void cModuleInterface::addSCPIMeasureCommand(QString cmdparent, QString cmd, quint8 cmdType, quint8 measCode, cSCPIMeasure *measureObject)
{
    cSCPIMeasureDelegate* delegate;
    QString cmdcomplete = QString("%1:%2").arg(cmdparent).arg(cmd);

    if (m_scpiMeasureDelegateHash.contains(cmdcomplete))
    {
        delegate = m_scpiMeasureDelegateHash.value(cmdcomplete);
        delegate->addscpimeasureObject(measureObject);
    }
    else
    {
        delegate = new cSCPIMeasureDelegate(cmdparent, cmd, cmdType , measCode, measureObject);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        m_pSCPIInterface->addSCPICommand(delegate);
    }
}

}

#include <veinentity.h>
#include <scpi.h>
#include <scpiobject.h>
#include <scpicommand.h>

#include "scpiinterface.h"
#include "scpinonmeasuredelegate.h"
#include "scpimeasure.h"
#include "scpimeasurecollector.h"
#include "scpimeasuredelegate.h"
#include "scpicmdinfo.h"

namespace SCPIMODULE
{

cSCPIInterface::cSCPIInterface(QString name)
    :m_sName(name)
{
    m_pSCPICmdInterface = new cSCPI(m_sName);
    m_nCmdCode = 0; //
}


cSCPIInterface::cSCPIInterface(const cSCPIInterface &other)
    :QObject()
{
    m_scpinonMeasureDelegateList = other.m_scpinonMeasureDelegateList;
    m_scpiMeasureDelegateHash = other.m_scpiMeasureDelegateHash;
    m_nCmdCode = other.m_nCmdCode;
    m_sName = other.m_sName;
}


cSCPIInterface::~cSCPIInterface()
{
    delete m_pSCPICmdInterface;
}


void cSCPIInterface::setAuthorisation(bool auth)
{
    m_bAuthorized = auth;
}


void cSCPIInterface::addSCPICommand(cSCPICmdInfo &scpiCmdInfo)
{
    if (scpiCmdInfo.scpiModel == "MEASURE")
    {
        // in case of measure model we add several commands by ourself

        cSCPIMeasure* measureObject = new cSCPIMeasure(scpiCmdInfo.entity);

        addSCPIMeasureCommand(QString("MEASURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, m_nCmdCode + SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, m_nCmdCode + SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, m_nCmdCode + SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, m_nCmdCode + SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, m_nCmdCode + SCPIModelType::fetch, measureObject);

        m_nCmdCode += 8;

        addSCPIMeasureCommand(QString("MEASURE"), scpiCmdInfo.scpiModuleName, SCPI::isQuery, m_nCmdCode + SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE"), scpiCmdInfo.scpiModuleName, SCPI::isCmd, m_nCmdCode + SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ"), scpiCmdInfo.scpiModuleName, SCPI::isQuery, m_nCmdCode + SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT"), scpiCmdInfo.scpiModuleName, SCPI::isCmd, m_nCmdCode + SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH"), scpiCmdInfo.scpiModuleName, SCPI::isQuery, m_nCmdCode + SCPIModelType::fetch, measureObject);

        m_nCmdCode += 8;

        addSCPIMeasureCommand(QString(""), QString("MEASURE"), SCPI::isQuery, m_nCmdCode + SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("CONFIGURE"), SCPI::isCmd, m_nCmdCode + SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("READ"), SCPI::isQuery, m_nCmdCode + SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString(""), QString("INIT"), SCPI::isCmd, m_nCmdCode + SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString(""), QString("FETCH"), SCPI::isQuery, m_nCmdCode + SCPIModelType::fetch, measureObject);
    }
    else
    {
        bool ok;
        cSCPInonMeasureDelegate* delegate;

        delegate = new cSCPInonMeasureDelegate(QString("%1:%2").arg(scpiCmdInfo.scpiModel).arg(scpiCmdInfo.scpiModuleName),
                                     scpiCmdInfo.cmdNode, scpiCmdInfo.type.toInt(&ok), m_pSCPICmdInterface, m_nCmdCode + SCPIModelType::normal, scpiCmdInfo.peer, scpiCmdInfo.entity);
        m_scpinonMeasureDelegateList.append(delegate);
        connect(delegate, SIGNAL(execute(int, QString&)), this, SLOT(executeCmd(int,QString&)));
    }

    m_nCmdCode+=8; // next command code

}


bool cSCPIInterface::executeCmd(QString cmd)
{
    cSCPIObject* scpiObject;
    QString dummy;

    if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd, dummy)) != 0)
    {
        cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
        return scpiDelegate->executeSCPI(cmd);
    }

    return false; // maybe that it is a common command
}


void cSCPIInterface::addSCPIMeasureCommand(QString cmdparent, QString cmd, quint8 cmdType, quint16 cmdCode, cSCPIMeasure *measureObject)
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
        delegate = new cSCPIMeasureDelegate(cmdparent, cmd, cmdType , m_pSCPICmdInterface, cmdCode, measureObject);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        connect(delegate, SIGNAL(execute(int, QString&)), this, SLOT(executeCmd(int,QString&)));
    }
}


void cSCPIInterface::executeCmd(int cmdCode, QString &sInput)
{
    quint8 cmdMeas, scpiCmdType;
    bool validation;

    cSCPICommand cmd = sInput;

    cmdMeas = cmdCode % 8;

    if (cmdMeas == 0) // so we have a non measure command
    {
        cSCPInonMeasureDelegate* delegate = qobject_cast<cSCPInonMeasureDelegate*>(sender());
        scpiCmdType = delegate->getType();
        VeinEntity* entity = delegate->getEntity();

        if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) // test if we got an allowed query
        {
            QString answer = entity->getValue().toString();
            emit cmdAnswer(answer);
        }
        else

            if (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) // test if we got an allowed cmd + 1 parameter
            {
                if (m_bAuthorized)
                {
                    if (validation = true) // todo here
                    {
                        QVariant newVar = cmd.getParam(0);
                        QVariant var = entity->getValue();
                        entity->setValue(newVar.convert(var.type()), delegate->getPeer());
                        emit cmdStatus(SCPI::ack);
                    }
                    else
                        emit cmdStatus(SCPI::errval);
                }
                else
                    emit cmdStatus(SCPI::erraut);
            }

            else
                emit cmdStatus(SCPI::nak);
    }

    else // otherwise we have to measure, configure, read, init or fetch something
    {
        cSCPIMeasureDelegate* delegate = qobject_cast<cSCPIMeasureDelegate*>(sender());
        scpiCmdType = delegate->getType();

        if ( (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) ||
             (cmd.isCommand(0) && ((scpiCmdType & SCPI::isCmd) > 0)) )
        {
            QList<cSCPIMeasure*>* measureObjectList = delegate->getscpimeasureObjectList();

            cSCPIMeasureCollector* measureCollector = new cSCPIMeasureCollector(measureObjectList->count());
            connect(measureCollector, SIGNAL(cmdStatus(quint8)), this, SIGNAL(cmdStatus(quint8)));
            connect(measureCollector, SIGNAL(cmdAnswer(QString)), this, SIGNAL(cmdAnswer(QString)));

            for (int i = 0; i < measureObjectList->count(); i++)
            {
                cSCPIMeasure* measure = measureObjectList->at(i);
                connect(measure, SIGNAL(cmdAnswer(QString)), measureCollector, SLOT(receiveAnswer(QString)));
                connect(measure, SIGNAL(cmdStatus(quint8)), measureCollector, SLOT(receiveStatus(quint8)));
                measure->execute(cmdMeas);
            }
        }
        else
            emit cmdStatus(SCPI::nak);
    }

}

}

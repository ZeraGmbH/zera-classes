#include <QString>
#include <QStateMachine>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

#include <veinpeer.h>
#include <veinhub.h>
#include <veinentity.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "modulesignal.h"
#include "interfaceentity.h"
#include "moduleparameter.h"
#include "moduleinfo.h"
#include "measmodeinfo.h"
#include "power3module.h"
#include "power3modulemeasprogram.h"
#include "power3measdelegate.h"


namespace POWER3MODULE
{

cPower3ModuleMeasProgram::cPower3ModuleMeasProgram(cPower3Module* module, VeinPeer* peer, cPower3ModuleConfigData& configdata)
    :cBaseMeasWorkProgram(peer), m_pModule(module), m_ConfigData(configdata)
{
    m_searchActualValuesState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_searchActualValuesState);

    connect(&m_searchActualValuesState, SIGNAL(entered()), SLOT(searchActualValues()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activateDone()));

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, SIGNAL(entered()), SLOT(deactivateMeas()));
    connect(&m_deactivateDoneState, SIGNAL(entered()), SLOT(deactivateMeasDone()));
}


cPower3ModuleMeasProgram::~cPower3ModuleMeasProgram()
{
    for (int i = 0; i < m_Power3MeasDelegateList.count(); i++)
        delete m_Power3MeasDelegateList.at(i);
}


void cPower3ModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cPower3ModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, 0);
}


void cPower3ModuleMeasProgram::generateInterface()
{
    VeinEntity* p_entity;
    QString s;

    // this here is for translation purpose
    s = tr("P%1;[W]");

    m_pHPWCountInfo = new cModuleInfo(m_pPeer, "INF_HPWCount", QVariant(m_ConfigData.m_nPowerSystemCount));

    for (int i = 0; i < m_ConfigData.m_nPowerSystemCount; i++) // we have a configures number of harmonic power values
    {
        s = QString("TRA_HPW%1Name").arg(i+1);
        p_entity = m_pPeer->dataAdd(s);
        p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
        p_entity->setValue(QVariant(QString("P%1;[W]").arg(i+1)), m_pPeer);
        m_EntityNameHPWList.append(p_entity);

        s = QString("ACT_HPW%1").arg(i+1); // actual value harmonic power
        p_entity = m_pPeer->dataAdd(s);
        p_entity->modifiersAdd(VeinEntity::MOD_READONLY);
        p_entity->setValue(QVariant((double) 0.0), m_pPeer);
        m_EntityActValueHPWList.append(p_entity);

    }

    m_pMeasureSignal = new cModuleSignal(m_pPeer, "SIG_MEASURING", QVariant(0));
}


void cPower3ModuleMeasProgram::deleteInterface()
{
    for (int i = 0; i < m_EntityNameHPWList.count(); i++)
        m_pPeer->dataRemove(m_EntityNameHPWList.at(i));
    for (int i = 0; i < m_EntityActValueHPWList.count(); i++)
        m_pPeer->dataRemove(m_EntityActValueHPWList.at(i));

    delete m_pHPWCountInfo;
    delete m_pMeasureSignal;
}


void cPower3ModuleMeasProgram::exportInterface(QJsonArray &jsArr)
{
    cInterfaceEntity ifaceEntity;

    ifaceEntity.setDescription(QString("This entity holds the power value of CmdNode")); // for all actvalues the same
    ifaceEntity.setSCPIModel(QString("MEASURE"));
    ifaceEntity.setSCPIType(QString("2"));

    for (int i = 0; i < m_ConfigData.m_nPowerSystemCount; i++)
    {
        ifaceEntity.setName(m_EntityActValueHPWList.at(i)->getName());

        QString chnDes = m_EntityNameHPWList.at(i)->getValue().toString();
        QStringList sl = chnDes.split(';');
        QString CmdNode = sl.takeFirst();
        QString Unit = sl.takeLast();

        ifaceEntity.setSCPICmdnode(CmdNode);
        ifaceEntity.setUnit(Unit);

        ifaceEntity.appendInterfaceEntity(jsArr);
    }
}


void cPower3ModuleMeasProgram::searchActualValues()
{
    VeinHub *vHub;
    VeinPeer *vPeerInputModule;
    bool error;

    error = false;
    vHub = m_pPeer->getHub();
    vPeerInputModule = vHub->getPeerByName(m_ConfigData.m_sInputModule);
    if (vPeerInputModule != 0)
    {
        for (int i = 0; i < m_ConfigData.m_nPowerSystemCount; i++)
        {
            VeinEntity *vEnt1, *vEnt2;
            vEnt1 = vPeerInputModule->getEntityByName(m_ConfigData.m_powerSystemConfigList.at(i).m_sInputU);
            vEnt2 = vPeerInputModule->getEntityByName(m_ConfigData.m_powerSystemConfigList.at(i).m_sInputI);

            // test that wanted input entities exist
            if ( (vEnt1 !=0) && (vEnt2 !=0))
            {
                cPower3MeasDelegate *cPMD;

                cPMD = new cPower3MeasDelegate(m_pPeer, m_EntityActValueHPWList.at(i));
                connect(vEnt1, SIGNAL(sigValueChanged(QVariant)), cPMD, SLOT(actValueInput1(QVariant)));
                connect(vEnt2, SIGNAL(sigValueChanged(QVariant)), cPMD, SLOT(actValueInput2(QVariant)));
            }
            else
                error = true;
        }
    }
    else
        error =true;

    if (error)
        emit activationError();
    else
        emit activationContinue();
}


void cPower3ModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}



void cPower3ModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;
    emit deactivationContinue();
}


void cPower3ModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}


}






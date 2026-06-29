#include "transformer1module.h"
#include "transformer1modulemeasprogram.h"
#include "transformer1measdelegate.h"
#include "errormessages.h"
#include <scpi.h>
#include <reply.h>
#include <vs_abstracteventsystem.h>
#include <doublevalidator.h>
#include <vfmodulemetadata.h>
#include <vfmoduleparameter.h>

namespace TRANSFORMER1MODULE
{

cTransformer1ModuleMeasProgram::cTransformer1ModuleMeasProgram(cTransformer1Module* module) :
    cBaseMeasWorkProgram(module->getVeinModuleName()),
    m_pModule(module)
{
    m_searchActualValuesState.addTransition(this, &cTransformer1ModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_searchActualValuesState);

    connect(&m_searchActualValuesState, &QState::entered, this, &cTransformer1ModuleMeasProgram::searchActualValues);
    connect(&m_activationDoneState, &QState::entered, this, &cTransformer1ModuleMeasProgram::activateDone);

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, &cTransformer1ModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, &QState::entered, this, &cTransformer1ModuleMeasProgram::deactivateMeas);
    connect(&m_deactivateDoneState, &QState::entered, this, &cTransformer1ModuleMeasProgram::deactivateMeasDone);
}


void cTransformer1ModuleMeasProgram::start()
{
}

void cTransformer1ModuleMeasProgram::stop()
{
}

void cTransformer1ModuleMeasProgram::generateVeinInterface()
{
    const cTransformer1ModuleConfigData *configData = m_pModule->getConfigData();
    for (int i = 0; i < configData->m_nTransformerSystemCount; i++) {
        VfModuleComponent *pActvalue;
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_Error%1").arg(i+1),
                                            QString("Transformer transmission error value"));
        pActvalue->setChannelName(QString("ERR%1").arg(i+1));
        pActvalue->setUnit("%");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_Angle%1").arg(i+1),
                                            QString("Transformer angle deviation value"));
        pActvalue->setChannelName(QString("ANG%1").arg(i+1));
        pActvalue->setUnit("°");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_Ratio%1").arg(i+1),
                                            QString("Transformer ratio value"));
        pActvalue->setChannelName(QString("RAT%1").arg(i+1));
        pActvalue->setUnit("");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_INSecondary%1").arg(i+1),
                                            QString("Reference N secondary input"));
        pActvalue->setChannelName(QString("INSEC%1").arg(i+1));
        pActvalue->setUnit(QString(configData->m_clampUnit[0]));
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_IXSecondary%1").arg(i+1),
                                            QString("DUT secondary input"));
        pActvalue->setChannelName(QString("IXSEC%1").arg(i+1));
        pActvalue->setUnit(QString(configData->m_clampUnit[2]));
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_IXPrimary%1").arg(i+1),
                                            QString("DUT test primary input"));
        pActvalue->setChannelName(QString("IXPRIM%1").arg(i+1));
        pActvalue->setUnit(QString(configData->m_clampUnit[4]));
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
    }

    QString key;
    m_pPrimClampPrimParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                         key = QString("PAR_PrimClampPrim"),
                                                         QString("Clamp primary value"),
                                                         QVariant(configData->primClampPrim.m_fValue));
    m_pPrimClampPrimParameter->setUnit(QString(configData->m_clampUnit[0]));
    m_pPrimClampPrimParameter->setScpiInfo("CONFIGURATION","PCPRIMARY", SCPI::isQuery|SCPI::isCmdwP);
    m_pPrimClampPrimParameter->setValidator(new cDoubleValidator(0.000001, 1000000.0, 0.000001));
    m_pModule->m_veinModuleParameterMap[key] = m_pPrimClampPrimParameter; // for modules use

    m_pPrimClampSecParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                        key = QString("PAR_PrimClampSec"),
                                                        QString("Primary clamp secondary value"),
                                                        QVariant(configData->primClampSec.m_fValue));
    m_pPrimClampSecParameter->setUnit(QString(configData->m_clampUnit[1]));
    m_pPrimClampSecParameter->setScpiInfo("CONFIGURATION","PCSECONDARY", SCPI::isQuery|SCPI::isCmdwP);
    m_pPrimClampSecParameter->setValidator(new cDoubleValidator(0.000001, 1000000.0, 0.000001));
    m_pModule->m_veinModuleParameterMap[key] = m_pPrimClampSecParameter; // for modules use

    m_pSecClampPrimParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                        key = QString("PAR_SecClampPrim"),
                                                        QString("Secondary clamp primary value"),
                                                        QVariant(configData->secClampPrim.m_fValue));
    m_pSecClampPrimParameter->setUnit(QString(configData->m_clampUnit[2]));
    m_pSecClampPrimParameter->setScpiInfo("CONFIGURATION","SCPRIMARY", SCPI::isQuery|SCPI::isCmdwP);
    m_pSecClampPrimParameter->setValidator(new cDoubleValidator(0.000001, 1000000.0, 0.000001));
    m_pModule->m_veinModuleParameterMap[key] = m_pSecClampPrimParameter; // for modules use

    m_pSecClampSecParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                       key = QString("PAR_SecClampSec"),
                                                       QString("Secondary clamp secondary value"),
                                                       QVariant(configData->secClampSec.m_fValue));
    m_pSecClampSecParameter->setUnit(QString(configData->m_clampUnit[3]));
    m_pSecClampSecParameter->setScpiInfo("CONFIGURATION","SCSECONDARY", SCPI::isQuery|SCPI::isCmdwP);
    m_pSecClampSecParameter->setValidator(new cDoubleValidator(0.000001, 1000000.0, 0.000001));
    m_pModule->m_veinModuleParameterMap[key] = m_pSecClampSecParameter; // for modules use

    m_pPrimDutParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                   key = QString("PAR_DutPrimary"),
                                                   QString("DUT primary value"),
                                                   QVariant(configData->dutPrim.m_fValue));
    m_pPrimDutParameter->setUnit(QString(configData->m_clampUnit[4]));
    m_pPrimDutParameter->setScpiInfo("CONFIGURATION","DUTPRIMARY", SCPI::isQuery|SCPI::isCmdwP);
    m_pPrimDutParameter->setValidator(new cDoubleValidator(0.000001, 1000000.0, 0.000001));
    m_pModule->m_veinModuleParameterMap[key] = m_pPrimDutParameter; // for modules use

    m_pSecDutParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                  key = QString("PAR_DutSecondary"),
                                                  QString("DUT secondary value"),
                                                  QVariant(configData->dutSec.m_fValue));
    m_pSecDutParameter->setUnit(QString(configData->m_clampUnit[5]));
    m_pSecDutParameter->setScpiInfo("CONFIGURATION","DUTSECONDARY", SCPI::isQuery|SCPI::isCmdwP);
    m_pSecDutParameter->setValidator(new cDoubleValidator(0.000001, 1000000.0, 0.000001));
    m_pModule->m_veinModuleParameterMap[key] = m_pSecDutParameter; // for modules use

    m_pTRSCountInfo = new VfModuleMetaData(QString("TRSCount"), QVariant(configData->m_nTransformerSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pTRSCountInfo);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pMeasureSignal);
}


void cTransformer1ModuleMeasProgram::searchActualValues()
{
    bool error = false;
    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();
    const cTransformer1ModuleConfigData *configData = m_pModule->getConfigData();
    for (int i = 0; i < configData->m_nTransformerSystemCount; i++) {
        VeinStorage::AbstractComponentPtr inputPrimaryVector =
            storageDb->findComponent(configData->m_nModuleId, configData->m_transformerSystemConfigList.at(i).m_sInputPrimaryVector);
        VeinStorage::AbstractComponentPtr inputSecondaryVector =
            storageDb->findComponent(configData->m_nModuleId, configData->m_transformerSystemConfigList.at(i).m_sInputSecondaryVector);
        if(inputPrimaryVector && inputSecondaryVector) {
            cTransformer1MeasDelegate *cTMD;
            if (i == (configData->m_nTransformerSystemCount-1)) {
                cTMD = new cTransformer1MeasDelegate(m_veinActValueList.at(i*6), m_veinActValueList.at(i*6+1), m_veinActValueList.at(i*6+2),
                                                     m_veinActValueList.at(i*6+3), m_veinActValueList.at(i*6+4), m_veinActValueList.at(i*6+5), true);
                connect(cTMD, &cTransformer1MeasDelegate::measuring, this, &cTransformer1ModuleMeasProgram::setMeasureSignal);
            }
            else
                cTMD = new cTransformer1MeasDelegate(m_veinActValueList.at(i*6), m_veinActValueList.at(i*6+1), m_veinActValueList.at(i*6+2),
                                                     m_veinActValueList.at(i*6+3), m_veinActValueList.at(i*6+4), m_veinActValueList.at(i*6+5));
            m_Transformer1MeasDelegateList.append(cTMD);

            connect(inputPrimaryVector.get(), &VeinStorage::AbstractComponent::sigValueChange,
                    cTMD, &cTransformer1MeasDelegate::actValueInput1);
            connect(inputSecondaryVector.get(), &VeinStorage::AbstractComponent::sigValueChange,
                    cTMD, &cTransformer1MeasDelegate::actValueInput2);
        }
        else
            error = true;
    }
    if (error)
        notifyError(confiuredVeinComponentsNotFound);
    else
        emit activationContinue();
}

void cTransformer1ModuleMeasProgram::activateDone()
{
    setParameters();

    connect(m_pPrimClampPrimParameter, &VfModuleParameter::sigValueChanged, this, &cTransformer1ModuleMeasProgram::newPrimClampPrim);
    connect(m_pPrimClampSecParameter, &VfModuleParameter::sigValueChanged, this, &cTransformer1ModuleMeasProgram::newPrimClampSec);
    connect(m_pSecClampPrimParameter, &VfModuleParameter::sigValueChanged, this, &cTransformer1ModuleMeasProgram::newSecClampPrim);
    connect(m_pSecClampSecParameter, &VfModuleParameter::sigValueChanged, this, &cTransformer1ModuleMeasProgram::newSecClampSec);
    connect(m_pPrimDutParameter, &VfModuleParameter::sigValueChanged, this, &cTransformer1ModuleMeasProgram::newPrimDut);
    connect(m_pSecDutParameter, &VfModuleParameter::sigValueChanged, this, &cTransformer1ModuleMeasProgram::newSecDut);

    m_bActive = true;
    emit activated();
}

void cTransformer1ModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;
    for (int i = 0; i < m_Transformer1MeasDelegateList.count(); i++)
        delete m_Transformer1MeasDelegateList.at(i);
    m_Transformer1MeasDelegateList.clear();
    emit deactivationContinue();
}

void cTransformer1ModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

void cTransformer1ModuleMeasProgram::setMeasureSignal(int signal)
{
    m_pMeasureSignal->setValue(signal);
}

void cTransformer1ModuleMeasProgram::newPrimClampPrim(QVariant pcp)
{
    m_pModule->getConfigData()->primClampPrim.m_fValue = pcp.toFloat();
    setParameters();
    emit m_pModule->parameterChanged();
}

void cTransformer1ModuleMeasProgram::newPrimClampSec(QVariant pcs)
{
    m_pModule->getConfigData()->primClampSec.m_fValue = pcs.toFloat();
    setParameters();
    emit m_pModule->parameterChanged();
}

void cTransformer1ModuleMeasProgram::newSecClampPrim(QVariant scp)
{
    m_pModule->getConfigData()->secClampPrim.m_fValue = scp.toFloat();
    setParameters();
    emit m_pModule->parameterChanged();
}

void cTransformer1ModuleMeasProgram::newSecClampSec(QVariant scs)
{
    m_pModule->getConfigData()->secClampSec.m_fValue = scs.toFloat();
    setParameters();
    emit m_pModule->parameterChanged();
}

void cTransformer1ModuleMeasProgram::newPrimDut(QVariant pd)
{
    m_pModule->getConfigData()->dutPrim.m_fValue = pd.toFloat();
    setParameters();
    emit m_pModule->parameterChanged();
}

void cTransformer1ModuleMeasProgram::newSecDut(QVariant sd)
{
    m_pModule->getConfigData()->dutSec.m_fValue = sd.toFloat();
    setParameters();
    emit m_pModule->parameterChanged();
}

void cTransformer1ModuleMeasProgram::setParameters()
{
    const cTransformer1ModuleConfigData *configData = m_pModule->getConfigData();
    for (int i = 0; i < m_Transformer1MeasDelegateList.count(); i++) {
        cTransformer1MeasDelegate* tmd = m_Transformer1MeasDelegateList.at(i);
        tmd->setPrimClampPrim(configData->primClampPrim.m_fValue);
        tmd->setPrimClampSec(configData->primClampSec.m_fValue);
        tmd->setSecClampPrim(configData->secClampPrim.m_fValue);
        tmd->setSecClampSec(configData->secClampSec.m_fValue);
        tmd->setDutPrim(configData->dutPrim.m_fValue);
        tmd->setDutSec(configData->dutSec.m_fValue);
    }
}

}




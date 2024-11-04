#include <QString>
#include <QStateMachine>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

#include <vs_abstracteventsystem.h>
#include <doublevalidator.h>
#include <vfmoduleactvalue.h>
#include <vfmodulemetadata.h>
#include <vfmoduleparameter.h>
#include <scpiinfo.h>

#include "errormessages.h"
#include <reply.h>
#include "transformer1module.h"
#include "transformer1modulemeasprogram.h"
#include "transformer1measdelegate.h"
#include "transformer1moduleconfiguration.h"


namespace TRANSFORMER1MODULE
{

cTransformer1ModuleMeasProgram::cTransformer1ModuleMeasProgram(cTransformer1Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration)
    :cBaseMeasWorkProgram(pConfiguration), m_pModule(module)
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


cTransformer1ModuleMeasProgram::~cTransformer1ModuleMeasProgram()
{
    // we delete transformer meas delegates on deactivation of module
}


void cTransformer1ModuleMeasProgram::start()
{
}


void cTransformer1ModuleMeasProgram::stop()
{
}

cTransformer1ModuleConfigData *cTransformer1ModuleMeasProgram::getConfData()
{
    return qobject_cast<cTransformer1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cTransformer1ModuleMeasProgram::generateInterface()
{
    VfModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;
    QString key, s;

    for (int i = 0; i < getConfData()->m_nTransformerSystemCount; i++)
    {
        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_Error%1").arg(i+1),
                                            QString("Transformer transmission error value"));
        pActvalue->setChannelName(QString("ERR%1").arg(i+1));
        pActvalue->setUnit("%");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_Angle%1").arg(i+1),
                                            QString("Transformer angle deviation value"));
        pActvalue->setChannelName(QString("ANG%1").arg(i+1));
        pActvalue->setUnit("°");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_Ratio%1").arg(i+1),
                                            QString("Transformer ratio value"));
        pActvalue->setChannelName(QString("RAT%1").arg(i+1));
        pActvalue->setUnit("");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_INSecondary%1").arg(i+1),
                                            QString("Reference N secondary input"));
        pActvalue->setChannelName(QString("INSEC%1").arg(i+1));
        pActvalue->setUnit(QString(getConfData()->m_clampUnit[0]));

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_IXSecondary%1").arg(i+1),
                                            QString("DUT secondary input"));
        pActvalue->setChannelName(QString("IXSEC%1").arg(i+1));
        pActvalue->setUnit(QString(getConfData()->m_clampUnit[2]));

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_IXPrimary%1").arg(i+1),
                                            QString("DUT test primary input"));
        pActvalue->setChannelName(QString("IXPRIM%1").arg(i+1));
        pActvalue->setUnit(QString(getConfData()->m_clampUnit[4]));

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    m_pPrimClampPrimParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                         key = QString("PAR_PrimClampPrim"),
                                                         QString("Clamp primary value"),
                                                         QVariant(getConfData()->primClampPrim.m_fValue));
    s = QString(getConfData()->m_clampUnit[0]);
    m_pPrimClampPrimParameter->setUnit(s);
    m_pPrimClampPrimParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","PCPRIMARY", "10", "PAR_PrimClampPrim", "0", s));

    cDoubleValidator *dValidator;
    dValidator = new cDoubleValidator(0.000001, 1000000.0, 0.000001);
    m_pPrimClampPrimParameter->setValidator(dValidator);

    m_pModule->m_veinModuleParameterMap[key] = m_pPrimClampPrimParameter; // for modules use

    m_pPrimClampSecParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                        key = QString("PAR_PrimClampSec"),
                                                        QString("Primary clamp secondary value"),
                                                        QVariant(getConfData()->primClampSec.m_fValue));
    s = QString(getConfData()->m_clampUnit[1]);
    m_pPrimClampSecParameter->setUnit(s);
    m_pPrimClampSecParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","PCSECONDARY", "10", "PAR_PrimClampSec", "0", s));

    dValidator = new cDoubleValidator(0.000001, 1000000.0, 0.000001);
    m_pPrimClampSecParameter->setValidator(dValidator);

    m_pModule->m_veinModuleParameterMap[key] = m_pPrimClampSecParameter; // for modules use

    m_pSecClampPrimParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                        key = QString("PAR_SecClampPrim"),
                                                        QString("Secondary clamp primary value"),
                                                        QVariant(getConfData()->secClampPrim.m_fValue));
    s = QString(getConfData()->m_clampUnit[2]);
    m_pSecClampPrimParameter->setUnit(s);
    m_pSecClampPrimParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","SCPRIMARY", "10", "PAR_SecClampPrim", "0", s));

    dValidator = new cDoubleValidator(0.000001, 1000000.0, 0.000001);
    m_pSecClampPrimParameter->setValidator(dValidator);

    m_pModule->m_veinModuleParameterMap[key] = m_pSecClampPrimParameter; // for modules use

    m_pSecClampSecParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_SecClampSec"),
                                                       QString("Secondary clamp secondary value"),
                                                       QVariant(getConfData()->secClampSec.m_fValue));
    s = QString(getConfData()->m_clampUnit[3]);
    m_pSecClampSecParameter->setUnit(s);
    m_pSecClampSecParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","SCSECONDARY", "10", "PAR_SecClampSec", "0", s));

    dValidator = new cDoubleValidator(0.000001, 1000000.0, 0.000001);
    m_pSecClampSecParameter->setValidator(dValidator);

    m_pModule->m_veinModuleParameterMap[key] = m_pSecClampSecParameter; // for modules use

    m_pPrimDutParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                   key = QString("PAR_DutPrimary"),
                                                   QString("DUT primary value"),
                                                   QVariant(getConfData()->dutPrim.m_fValue));
    s = QString(getConfData()->m_clampUnit[4]);
    m_pPrimDutParameter->setUnit(s);
    m_pPrimDutParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","DUTPRIMARY", "10", "PAR_DutPrimary", "0", s));

    dValidator = new cDoubleValidator(0.000001, 1000000.0, 0.000001);
    m_pPrimDutParameter->setValidator(dValidator);

    m_pModule->m_veinModuleParameterMap[key] = m_pPrimDutParameter; // for modules use

    m_pSecDutParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                  key = QString("PAR_DutSecondary"),
                                                  QString("DUT secondary value"),
                                                  QVariant(getConfData()->dutSec.m_fValue));
    s = QString(getConfData()->m_clampUnit[5]);
    m_pSecDutParameter->setUnit(s);
    m_pSecDutParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","DUTSECONDARY", "10", "PAR_DutSecondary", "0", s));

    dValidator = new cDoubleValidator(0.000001, 1000000.0, 0.000001);
    m_pSecDutParameter->setValidator(dValidator);

    m_pModule->m_veinModuleParameterMap[key] = m_pSecDutParameter; // for modules use

    m_pTRSCountInfo = new VfModuleMetaData(QString("TRSCount"), QVariant(getConfData()->m_nTransformerSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pTRSCountInfo);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cTransformer1ModuleMeasProgram::searchActualValues()
{
    bool error = false;
    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageSystem()->getDb();
    for (int i = 0; i < getConfData()->m_nTransformerSystemCount; i++) {
        VeinStorage::AbstractComponentPtr inputPrimaryVector =
            storageDb->findComponent(getConfData()->m_nModuleId, getConfData()->m_transformerSystemConfigList.at(i).m_sInputPrimaryVector);
        VeinStorage::AbstractComponentPtr inputSecondaryVector =
            storageDb->findComponent(getConfData()->m_nModuleId, getConfData()->m_transformerSystemConfigList.at(i).m_sInputSecondaryVector);
        if(inputPrimaryVector && inputSecondaryVector) {
            cTransformer1MeasDelegate *cTMD;
            if (i == (getConfData()->m_nTransformerSystemCount-1)) {
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
        emit activationError();
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
    bool ok;
    getConfData()->primClampPrim.m_fValue = pcp.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cTransformer1ModuleMeasProgram::newPrimClampSec(QVariant pcs)
{
    bool ok;
    getConfData()->primClampSec.m_fValue = pcs.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cTransformer1ModuleMeasProgram::newSecClampPrim(QVariant scp)
{
    bool ok;
    getConfData()->secClampPrim.m_fValue = scp.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cTransformer1ModuleMeasProgram::newSecClampSec(QVariant scs)
{
    bool ok;
    getConfData()->secClampSec.m_fValue = scs.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cTransformer1ModuleMeasProgram::newPrimDut(QVariant pd)
{
    bool ok;
    getConfData()->dutPrim.m_fValue = pd.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cTransformer1ModuleMeasProgram::newSecDut(QVariant sd)
{
    bool ok;
    getConfData()->dutSec.m_fValue = sd.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cTransformer1ModuleMeasProgram::setParameters()
{
    // we set the parameters here
    for (int i = 0; i < m_Transformer1MeasDelegateList.count(); i++)
    {
        cTransformer1MeasDelegate* tmd = m_Transformer1MeasDelegateList.at(i);
        tmd->setPrimClampPrim(getConfData()->primClampPrim.m_fValue);
        tmd->setPrimClampSec(getConfData()->primClampSec.m_fValue);
        tmd->setSecClampPrim(getConfData()->secClampPrim.m_fValue);
        tmd->setSecClampSec(getConfData()->secClampSec.m_fValue);
        tmd->setDutPrim(getConfData()->dutPrim.m_fValue);
        tmd->setDutSec(getConfData()->dutSec.m_fValue);
    }
}

}




#include "thdnshadowmodulemeasprogram.h"
#include "servicechannelnamehelper.h"
#include "thdnmoduleconfiguration.h"
#include <doublevalidator.h>
#include <vf_client_component_setter.h>
#include <scpi.h>

namespace THDNMODULE
{

cThdnShadowModuleMeasProgram::cThdnShadowModuleMeasProgram(cThdnModule *module,
                                                           const std::shared_ptr<BaseModuleConfiguration> &configuration) :
    cBaseMeasProgram(configuration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_activationDoneState);
    connect(&m_activationDoneState, &QAbstractState::entered, this, [&]() {
        m_bActive = true;
        m_pMeasureSignal->setValue(QVariant(1));
        connect(m_pIntegrationTimeParameter, &VfModuleComponent::sigValueChanged, this, &cThdnShadowModuleMeasProgram::newIntegrationtime);
        emit activated();
    });

    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationDoneState);
    connect(&m_deactivationDoneState, &QAbstractState::entered, this, [&]() {
        m_bActive = false;
        emit deactivated();
    });
}

void cThdnShadowModuleMeasProgram::generateVeinInterface()
{
    ChannelRangeObserver::SystemObserverPtr observer = m_pModule->getSharedChannelRangeObserver();
    const VeinStorage::AbstractDatabase *storageDb = m_pModule->getStorageDb();
    const QStringList channelMNames = observer->getChannelMNames();
    int sourceEntityId = getConfData()->m_thdrSourceEntity;
    for (int i = 0; i < channelMNames.size(); i++) {
        const QString channelMName = channelMNames[i];
        ServiceChannelNameHelper::TChannelAliasUnit aliasUnit =
            ServiceChannelNameHelper::getChannelAndUnit(channelMName, observer);
        const QString &channelAlias = aliasUnit.m_channelAlias;

        VfModuleComponent *pActvalue;
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                          QString("ACT_THDR%1").arg(i+1),
                                          QString("THDr actual value"));
        pActvalue->setChannelName(channelAlias);
        pActvalue->setUnit("%");
        pActvalue->setScpiInfo("MEASURE", channelAlias, SCPI::isCmdwP);
        m_veinActValueList.append(pActvalue);
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
        m_sourceActValueList.append(storageDb->findComponent(sourceEntityId, QString("ACT_THDR%1").arg(i+1)));
    }
    QString key;
    m_pIntegrationTimeParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                        key = QString("PAR_Interval"),
                                                        QString("Integration time"),
                                                        QVariant(getConfData()->m_fMeasInterval.m_fValue));
    m_pIntegrationTimeParameter->setUnit("s");
    m_pIntegrationTimeParameter->setScpiInfo("CONFIGURATION","TINTEGRATION", SCPI::isQuery|SCPI::isCmdwP);
    m_pIntegrationTimeParameter->setValidator(new cDoubleValidator(1.0, 100.0, 0.5));
    m_pModule->m_veinModuleParameterMap[key] = m_pIntegrationTimeParameter; // for modules use
    m_sourceIntegrationTimeParameter = storageDb->findComponent(sourceEntityId, "PAR_Interval");

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             QString("SIG_Measuring"),
                                             QString("Signal indicating measurement activity"),
                                             QVariant(0));
    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pMeasureSignal);
    m_sourceMeasSignal = storageDb->findComponent(sourceEntityId, "SIG_Measuring");
}

void cThdnShadowModuleMeasProgram::start()
{
    if (m_started)
        return;

    for (int i=0; i<m_veinActValueList.count(); ++i)
        connect(m_sourceActValueList[i].get(), &VeinStorage::AbstractComponent::sigValueChange,
                m_veinActValueList[i], &VfModuleComponent::setValue);
    connect(m_sourceIntegrationTimeParameter.get(), &VeinStorage::AbstractComponent::sigValueChange,
            m_pIntegrationTimeParameter, &VfModuleComponent::setValue);
    connect(m_sourceMeasSignal.get(), &VeinStorage::AbstractComponent::sigValueChange,
            m_pMeasureSignal, &VfModuleComponent::setValue);
    m_started = true;
}

void cThdnShadowModuleMeasProgram::stop()
{
    if (!m_started)
        return;

    for (int i=0; i<m_veinActValueList.count(); ++i)
        disconnect(m_sourceActValueList[i].get(), &VeinStorage::AbstractComponent::sigValueChange,
                   m_veinActValueList[i], &VfModuleComponent::setValue);
    disconnect(m_sourceIntegrationTimeParameter.get(), &VeinStorage::AbstractComponent::sigValueChange,
               m_pIntegrationTimeParameter, &VfModuleComponent::setValue);
    disconnect(m_sourceMeasSignal.get(), &VeinStorage::AbstractComponent::sigValueChange,
               m_pMeasureSignal, &VfModuleComponent::setValue);
    m_started = false;
}

void cThdnShadowModuleMeasProgram::newIntegrationtime(const QVariant &ti)
{
    int sourceEntityId = getConfData()->m_thdrSourceEntity;
    emit m_pModule->getValidatorEventSystem()->sigSendEvent(
        VfClientComponentSetter::generateEvent(sourceEntityId, "PAR_Interval", m_sourceIntegrationTimeParameter->getValue(), ti));
}

cThdnModuleConfigData *cThdnShadowModuleMeasProgram::getConfData()
{
    return qobject_cast<cThdnModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

}

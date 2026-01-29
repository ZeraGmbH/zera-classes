#include "samplemodulemeasprogram.h"
#include "samplemodule.h"
#include "samplemoduleconfiguration.h"
#include <taskpllchannelset.h>
#include <boolvalidator.h>
#include <stringvalidator.h>
#include <scpi.h>

namespace SAMPLEMODULE
{

cSampleModuleMeasProgram::cSampleModuleMeasProgram(cSampleModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module),
    m_pConfiguration(pConfiguration),
    m_obsermaticConfig(getConfData()->m_ObsermaticConfPar),
    m_pcbConnection(m_module->getNetworkConfig()),
    m_pllAutomatic(m_module->getStorageDb(), m_obsermaticConfig.m_pllChannelList)
{
    connect(&m_pllAutomatic, &PllAutomatic::sigSelectPllChannel,
            this, &cSampleModuleMeasProgram::onPllChannelChanged);
}

void cSampleModuleMeasProgram::generateVeinInterface()
{
    QString key;
    m_pVeinPllChannelAlias = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                   key = QString("PAR_PllChannel"),
                                                   QString("PLL reference channel"),
                                                   QVariant(getAlias(m_obsermaticConfig.m_pllSystemChannel.m_sPar)));
    m_module->m_veinModuleParameterMap[key] = m_pVeinPllChannelAlias; // for modules use
    m_pVeinPllChannelAlias->setScpiInfo("CONFIGURATION", "PLLREFERENCE", SCPI::isQuery|SCPI::isCmdwP, m_pVeinPllChannelAlias->getName());
    setPllChannelValidator();

    m_pParPllAutomaticOnOff = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                    key = QString("PAR_PllAutomaticOnOff"),
                                                    QString("PLL automatic on/off"),
                                                    QVariant(m_obsermaticConfig.m_npllAutoAct.m_nActive));
    m_module->m_veinModuleParameterMap[key] = m_pParPllAutomaticOnOff; // for modules use
    m_pParPllAutomaticOnOff->setValidator(new cBoolValidator());
    m_pParPllAutomaticOnOff->setScpiInfo("CONFIGURATION", "PLLAUTO", SCPI::isQuery|SCPI::isCmdwP, m_pParPllAutomaticOnOff->getName());

    m_pPllFixed = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                        QString("ACT_PllFixed"),
                                        QString("PLL fixed channel / mode"),
                                        QVariant(m_obsermaticConfig.m_bpllFixed));
    m_module->m_veinComponentsWithMetaAndScpi.append(m_pPllFixed);
}

void cSampleModuleMeasProgram::activate()
{
    connect(m_pParPllAutomaticOnOff, &VfModuleParameter::sigValueChanged, this, &cSampleModuleMeasProgram::onVeinPllAutoChanged);
    connect(m_pVeinPllChannelAlias, &VfModuleParameter::sigValueChanged, this, &cSampleModuleMeasProgram::onVeinPllChannelChanged);

    QObject *contextForDisconnect = new QObject(this); // https://stackoverflow.com/questions/14828678/disconnecting-lambda-functions-in-qt5
    connect(&m_pendingTasks, &TaskTemplate::sigFinish, contextForDisconnect, [&, contextForDisconnect](bool ok) {
        delete contextForDisconnect;
        if (!ok) {
            qWarning("Something went wrong on sample module activation!");
            return;
        }
        m_bActive = true;
        m_pllAutomatic.activate(m_obsermaticConfig.m_npllAutoAct.m_nActive);
        emit activated();
    });
    m_pendingTasks.addSub(m_pcbConnection.createConnectionTask());
    startSetPllChannel(m_obsermaticConfig.m_pllSystemChannel.m_sPar);
}

void cSampleModuleMeasProgram::deactivate()
{
    m_bActive = false;
    emit deactivated();
}

void cSampleModuleMeasProgram::onVeinPllChannelChanged(QVariant channelAlias)
{
    if (m_obsermaticConfig.m_npllAutoAct.m_nActive != 0) {
        qWarning("Cannot set PLL channel when automatic mode is on!");
        m_pVeinPllChannelAlias->setValue(getAlias(m_obsermaticConfig.m_pllSystemChannel.m_sPar));
        return;
    }
    else if (m_obsermaticConfig.m_bpllFixed) {
        qWarning("Cannot set PLL channel on fixed frequency!");
        m_pVeinPllChannelAlias->setValue("");
        return;
    }
    QString channelMName = m_module->getSharedChannelRangeObserver()->getChannelMName(channelAlias.toString());
    trySendPllChannel(channelMName);
}

void cSampleModuleMeasProgram::onVeinPllAutoChanged(QVariant pllauto)
{
    bool pllAutoOn = pllauto.toBool();
    if (pllAutoOn && m_obsermaticConfig.m_bpllFixed) {
        qWarning("Cannot active PLL channel on fixed frequency!");
        m_pParPllAutomaticOnOff->setValue(0);
        return;
    }

    m_pllAutomatic.activate(pllAutoOn);
    m_obsermaticConfig.m_npllAutoAct.m_nActive = pllAutoOn;
    emit m_module->parameterChanged();
}

void cSampleModuleMeasProgram::onPllChannelChanged(QString channelMName)
{
    trySendPllChannel(channelMName);
}

cSampleModuleConfigData *cSampleModuleMeasProgram::getConfData()
{
    return qobject_cast<cSampleModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

QString cSampleModuleMeasProgram::getAlias(const QString &channelMName)
{
    ChannelRangeObserver::SystemObserverPtr observer = m_module->getSharedChannelRangeObserver();
    if (!observer->getChannelMNames().contains(channelMName))
        return QString();
    ChannelRangeObserver::ChannelPtr channel = observer->getChannel(channelMName);
    return channel->getAlias();
}

void cSampleModuleMeasProgram::setPllChannelValidator()
{
    const QStringList &channelMNames = m_obsermaticConfig.m_pllChannelList;
    QStringList channelAliases;
    for (const QString &pllChannelMName : channelMNames) {
        QString channelAlias = getAlias(pllChannelMName);
        if (!channelAlias.isEmpty())
            channelAliases.append(channelAlias);
    }
    cStringValidator *sValidator = new cStringValidator(channelAliases);
    m_pVeinPllChannelAlias->setValidator(sValidator);
}

void cSampleModuleMeasProgram::trySendPllChannel(const QString &channelMName)
{
    if(m_bActive && !m_obsermaticConfig.m_bpllFixed) {
        if(channelMName != m_obsermaticConfig.m_pllSystemChannel.m_sPar)
            startSetPllChannel(channelMName);
    }
}

void cSampleModuleMeasProgram::startSetPllChannel(const QString &channelMName)
{
    TaskTemplatePtr task = TaskPllChannelSet::create(m_pcbConnection.getInterface(),
                                                     channelMName,
                                                     TRANSACTION_TIMEOUT,
                                                     [=]() { qWarning("An error occurred setting PLL channel %s", qPrintable(channelMName));}
                                                     );
    connect(task.get(), &TaskTemplate::sigFinish, [=] () {
        setVeinPllChannelPesistent(channelMName);
    });
    m_pendingTasks.addSub(std::move(task));
}

void cSampleModuleMeasProgram::setVeinPllChannelPesistent(const QString &channelMName)
{
    if(m_obsermaticConfig.m_pllSystemChannel.m_sPar != channelMName) {
        m_pVeinPllChannelAlias->setValue(getAlias(channelMName));
        m_obsermaticConfig.m_pllSystemChannel.m_sPar = channelMName;
        emit m_module->parameterChanged();
    }
}

}

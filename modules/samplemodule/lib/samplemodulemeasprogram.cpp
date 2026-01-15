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
    m_pcbConnection(m_module->getNetworkConfig())
{
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
    m_module->veinModuleComponentList.append(m_pPllFixed);

    m_pPllSignal = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                         QString("SIG_PLL"),
                                         QString("Signal on pll channel changing"),
                                         QVariant(0));
    m_module->veinModuleComponentList.append(m_pPllSignal);
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
    if(m_obsermaticConfig.m_npllAutoAct.m_nActive != 0) {
        qWarning("Cannot set PLL channel when automatic mode is on!");
        m_pVeinPllChannelAlias->setValue(getAlias(m_obsermaticConfig.m_pllSystemChannel.m_sPar));
        return;
    }
    else if(m_obsermaticConfig.m_bpllFixed) {
        qWarning("Cannot set PLL channel on fixed frequency!");
        m_pVeinPllChannelAlias->setValue("");
        return;
    }
    trySendPllChannel(channelAlias.toString());
}

void cSampleModuleMeasProgram::onVeinPllAutoChanged(QVariant pllauto)
{
    if(!m_obsermaticConfig.m_bpllFixed) {
        if ( pllauto.toInt() == 1) {
            m_obsermaticConfig.m_npllAutoAct.m_nActive = 1;
            pllAutomatic(); // call once if switched to automatic
        }
        else
            m_obsermaticConfig.m_npllAutoAct.m_nActive = 0;
        emit m_module->parameterChanged();
    }
}

cSampleModuleConfigData *cSampleModuleMeasProgram::getConfData()
{
    return qobject_cast<cSampleModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

QString cSampleModuleMeasProgram::getAlias(const QString &channelMName)
{
    ChannelRangeObserver::ChannelPtr channel = m_module->getSharedChannelRangeObserver()->getChannel(channelMName);
    if (channel == nullptr)
        return QString();
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

void cSampleModuleMeasProgram::pllAutomatic()
{
    if (m_bActive &&
        m_obsermaticConfig.m_npllAutoAct.m_nActive == 1 &&
        !m_obsermaticConfig.m_bpllFixed) {
        int i;
        int n = m_obsermaticConfig.m_pllChannelList.count();
        for (i = 0; i < n; i++) {
            /*double urValue = m_pllMeasChannelHash[ m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i)]]->getUrValue();
            if (m_ActualValues[i] > urValue * 10.0 / 100.0)
                break;*/
        }
        if (i == n)
            i = 0; // if none of our channels has 10% attenuation we take the first channel
        // now we set our new pll channel
        const QString newChannelMName = m_obsermaticConfig.m_pllChannelList.at(i);
        const QString newChannelAlias = getAlias(newChannelMName);
        if (!newChannelAlias.isEmpty())
            trySendPllChannel(newChannelAlias);
    }
}

void cSampleModuleMeasProgram::trySendPllChannel(const QString &channelAlias)
{
    if(m_bActive && !m_obsermaticConfig.m_bpllFixed) {
        QString channelMName = m_module->getSharedChannelRangeObserver()->getChannelMName(channelAlias);
        if(channelMName != m_obsermaticConfig.m_pllSystemChannel.m_sPar)
            startSetPllChannel(channelMName);
    }
}

void cSampleModuleMeasProgram::startSetPllChannel(const QString &channelMName)
{
    m_pPllSignal->setValue(1); // start pll change
    TaskTemplatePtr task = TaskPllChannelSet::create(m_pcbConnection.getInterface(),
                                                     channelMName,
                                                     TRANSACTION_TIMEOUT,
                                                     [=]() { qWarning("An error occurred setting PLL channel %s", qPrintable(channelMName));}
                                                     );
    connect(task.get(), &TaskTemplate::sigFinish, [=] () {
        setVeinPllChannelPesistent(channelMName);
        m_pPllSignal->setValue(0); // pll change finished
    });
    m_pendingTasks.addSub(std::move(task));
    m_pendingTasks.start();
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

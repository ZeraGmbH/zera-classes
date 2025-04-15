#include "rangemodule.h"
#include "rangemodulemeasprogram.h"
#include "rangeobsermatic.h"
#include "rangemeaschannelconvenience.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <regexvalidator.h>
#include <intvalidator.h>
#include <doublevalidator.h>
#include <boolvalidator.h>
#include <math.h>
#include <scpi.h>

namespace RANGEMODULE
{

cRangeObsermatic::cRangeObsermatic(cRangeModule *module,
                                   QList<QStringList> groupList,
                                   cObsermaticConfPar& confpar) :
    cModuleActivist(QString("RangeObsermatic / %1").arg(qPrintable(module->getVeinModuleName()))),
    m_pModule(module),
    m_GroupList(groupList),
    m_ConfPar(confpar)
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceRangeObser(
        m_pModule->getEntityId(),
        m_pModule->getSharedChannelRangeObserver()->getChannelMNames(),
        false /* just for demo COM5003 ref-session - postpone better solution now */);

    m_readGainCorrState.addTransition(this, &cRangeObsermatic::activationContinue, &m_readGainCorrDoneState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_readGainCorrState);
    m_activationMachine.addState(&m_readGainCorrDoneState);

    connect(&m_dspserverConnectState, &QState::entered, this, &cRangeObsermatic::dspserverConnect);
    connect(&m_readGainCorrState, &QState::entered, this, &cRangeObsermatic::readGainScale);
    connect(&m_readGainCorrDoneState, &QState::entered, this, &cRangeObsermatic::readGainScaleDone);

    m_deactivationInitState.addTransition(this, &cRangeObsermatic::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_activationMachine.setInitialState(&m_dspserverConnectState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);

    connect(&m_deactivationInitState, &QState::entered, this, &cRangeObsermatic::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &cRangeObsermatic::deactivationDone);

    m_writeGainScaleState.addTransition(this, &cRangeObsermatic::activationContinue, &m_writeGainScaleDoneState);
    m_writeGainScaleState.addTransition(this, &cRangeObsermatic::activationRepeat, &m_writeGainScaleRepeatState);
    m_writeGainScaleRepeatState.addTransition(this, &cRangeObsermatic::activationContinue, &m_writeGainScaleState);
    m_writeDspGainScaleStateMachine.addState(&m_writeGainScaleState);
    m_writeDspGainScaleStateMachine.addState(&m_writeGainScaleRepeatState);
    m_writeDspGainScaleStateMachine.addState(&m_writeGainScaleDoneState);
    m_writeDspGainScaleStateMachine.setInitialState(&m_writeGainScaleState);
    connect(&m_writeGainScaleState, &QState::entered, this, &cRangeObsermatic::writeGainScale);
}

void cRangeObsermatic::ActionHandler(QVector<float> *actualValues)
{
    Q_UNUSED(actualValues)
    if (m_nRangeSetPending == 0) {
        if (m_nWaitAfterRanging > 0) {
            m_nWaitAfterRanging--;
        }
        else {
            rangeObservation(); // first we test for overload conditions
            rangeAutomatic(); // let rangeautomatic do its job
            groupHandling(); // and look for grouping channels if necessary
            setRanges(); // set the new ranges now
        }
    }
}


void cRangeObsermatic::generateVeinInterface()
{
    VfModuleComponent *pComponent;
    VfModuleParameter *pParameter;

    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    const QString unknownStr = "Unkown";
    for (int i = 0; i < channelMNames.count(); i++) {
        QString key = QString("PAR_Channel%1Range").arg(i+1);
        pParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                           key,
                                           QString("Channel's range"),
                                           unknownStr,
                                           true); // we prefer deferred notification for synchronization purpose

        m_RangeParameterList.append(pParameter); // for internal use
        m_pModule->m_veinModuleParameterMap[key] = pParameter; // for modules use

        m_actChannelRangeList.append(unknownStr); // here we also fill our internal actual channel range list
        m_RangeMeasChannelList.append(m_pModule->getMeasChannel(channelMNames.at(i)));

        pComponent = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              QString("SIG_Channel%1OVL").arg(i+1),
                                              QString("Channels overload status"),
                                              QVariant(int(0)) );

        m_RangeOVLComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        pComponent = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              QString("INF_Channel%1OVLREJ").arg(i+1),
                                              QString("Channel's maximum range peak rejection"),
                                              QVariant(double(0.0)) );

        m_RangeOVLRejectionComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        pComponent = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              QString("INF_Channel%1ActREJ").arg(i+1),
                                              QString("Channel's actual rejection"),
                                              QVariant(double(0.0)) );

        m_RangeActRejectionComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        pComponent = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              QString("INF_Channel%1ActOVLREJ").arg(i+1),
                                              QString("Channel's actual range peak rejection"),
                                              QVariant(double(0.0)) );

        m_RangeActOVLRejectionComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        m_softOvlList.append(false);
        m_hardOvlList.append(false);
        m_groupOvlList.append(false);
        m_maxOvlList.append(false);
    }

    for(int i=0; i < m_GroupList.length();++i) {
        QString key = QString("PAR_PreScalingGroup%1").arg(i);
        pParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key,
                                             QString("Scaling factor for group k (see configuration)"),
                                             QVariant("1/1"),
                                             false);

        pParameter->setUnit("");
        pParameter->setValidator(new cRegExValidator("^[1-9][0-9]*\\/[1-9][0-9]*(\\*\\((1|sqrt\\(3\\)|1\\/sqrt\\(3\\))\\))?$"));
        if(m_GroupList.at(i).contains("UL1"))
            pParameter->setValue("1/1*(1)");

        m_RangeGroupPreScalingList.append(pParameter);
        m_pModule->m_veinModuleParameterMap[key] = pParameter;

        // activate preScaling for U and I
        pParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("PAR_PreScalingEnabledGroup%1").arg(i),
                                             QString("Enable prescaling for group"),
                                             QVariant(0),
                                             false);

        pParameter->setValidator(new cBoolValidator());
        pParameter->setUnit("");

        m_RangeGroupPreScalingEnabledList.append(pParameter);
        m_pModule->m_veinModuleParameterMap[key] = pParameter;

        pComponent = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              QString("INF_PreScalingInfoGroup%1").arg(i),
                                              QString("Channel's actual range peak rejection"),
                                              QVariant(double(1.0)) );


        m_RangeGroupPreScalingInfo.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

    }

    if (m_GroupList.count() > 0) {
        for (int i = 0; i < m_GroupList.count(); i++) {
            QString groupMems = m_GroupList.at(i).join(";");
            VfModuleMetaData *pMetaData = new VfModuleMetaData(QString("ChannelGroup%1").arg(i+1), QVariant(groupMems));
            m_pModule->veinModuleMetaDataList.append(pMetaData); // only for module use
        }
    }

    m_pParRangeAutomaticOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                         QString("PAR_RangeAutomatic"),
                                                         QString("Range automatic on/off"),
                                                         QVariant(0));

    m_pModule->m_veinModuleParameterMap["PAR_RangeAutomatic"] = m_pParRangeAutomaticOnOff; // for modules use

    m_pParGroupingOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                   QString("PAR_ChannelGrouping"),
                                                   QString("Channel grouping on/off"),
                                                   QVariant(0));

    m_pModule->m_veinModuleParameterMap["PAR_ChannelGrouping"] = m_pParGroupingOnOff; // for modules use

    m_pParOverloadOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                   QString("PAR_Overload"),
                                                   QString("Overload"),
                                                   QVariant(0));

    m_pModule->m_veinModuleParameterMap["PAR_Overload"] = m_pParOverloadOnOff; // for modules use

    m_pComponentOverloadMax = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                       QString("INF_OverloadMax"),
                                                       QString("Maximum range overload condition"),
                                                       QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pComponentOverloadMax);

    m_pRangingSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                QString("SIG_Ranging"),
                                                QString("Signal on changing ranges"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pRangingSignal);
}


void cRangeObsermatic::handleOverload(const int channelIdx, bool rmsOverload, bool hardOverLoad, bool adcOverLoad)
{
    cRangeMeasChannel *rangeMeasChannel = m_RangeMeasChannelList.at(channelIdx);
    qInfo("Overload channel %i / Range %s: RMS %i / ADC %i / Hard %i",
          channelIdx,
          qPrintable(m_actChannelRangeList.at(channelIdx)),
          rmsOverload,
          adcOverLoad,
          hardOverLoad);
    // we mark each overload condition if possible (range automatic) we unmark it
    // but there was an edge on this entity

    // if an overload is recovered by rangeautomatic during running measurement
    QString maxRangeAlias = rangeMeasChannel->getMaxRange(m_ConfPar.getCurrentRange(channelIdx));
    if (m_actChannelRangeList.at(channelIdx) == maxRangeAlias) { // in case ovrload was in max. range
        m_maxOvlList.replace(channelIdx, true);
        m_pComponentOverloadMax->setValue(1);
    }

    m_ConfPar.setCurrentRange(channelIdx, maxRangeAlias); // we preset the max. range here
    m_RangeOVLComponentList.at(channelIdx)->setValue(QVariant(1)); // set interface overload
    m_softOvlList.replace(channelIdx, true);
}

void cRangeObsermatic::rangeObservation()
{
    bool markOverload = false;
    qint32 nrActValues = m_RangeMeasChannelList.count();
    for (int i = 0; i < nrActValues; i++) { // we test all channels
        cRangeMeasChannel *rangeMeasChannel = m_RangeMeasChannelList.at(i);
        // the overload observation must consider the prescaling
        float prescalingFac = getPreScale(i);
        // for test overload we take the rms value with/without dc depending on configuration
        // and for overload condition of adc test, we take the peakvalues including dc
        const RangeChannelData *channelData = rangeMeasChannel->getChannelData();
        bool rmsOverload = rangeMeasChannel->isRMSOverload(channelData->getRmsValue()*prescalingFac); // rms
        bool adcOverLoad = rangeMeasChannel->isADCOverload(channelData->getPeakValueWithDc()*prescalingFac); //peak
        bool hardOverLoad = m_hardOvlList.at(i);
        if ( rmsOverload || adcOverLoad || hardOverLoad) { // if any overload ?
            markOverload = true;
            handleOverload(i, rmsOverload, hardOverLoad, adcOverLoad);
        }
        else {
            m_RangeOVLComponentList.at(i)->setValue(QVariant(0));
            m_softOvlList.replace(i, false);
        }
    }
    setOverloadVeinComponent(markOverload);
}


void cRangeObsermatic::rangeAutomatic()
{
    if (m_ConfPar.m_nRangeAutoAct.m_nActive == 1) {
        bool unmarkOverload = true;

        for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we test all channels
            if (!m_maxOvlList.at(i)) { // no range automatic if there was overload in max range
                cRangeMeasChannel *rangeMeasChannel = m_RangeMeasChannelList.at(i);

                if (!m_hardOvlList.at(i)) {
                    if (!m_softOvlList.at(i)) {
                        const QString range = m_ConfPar.getCurrentRange(i);
                        const RangeChannelData *channelData = rangeMeasChannel->getChannelData();
                        const QString optRange = rangeMeasChannel->getOptRange(channelData->getPeakValue()*getPreScale(i), range);
                        m_ConfPar.setCurrentRange(i, optRange);
                    }
                }

                else {
                    unmarkOverload = false;
                    m_MsgNrCmdList[rangeMeasChannel->resetStatus()] = resetstatus;
                }
            }
            else {
                unmarkOverload = false;
            }
        }

        if (unmarkOverload) { // if we could recover all overloads
            m_pParOverloadOnOff->setValue(QVariant(int(0)));
        }
    }
}


void cRangeObsermatic::groupHandling()
{
    // preset group overload flags in any case
    for(auto &entry : m_groupOvlList) {
        entry = false;
    }
    if (m_ConfPar.m_nGroupAct.m_nActive == 1) {
        QList<int> indexList;
        const QStringList channelAliases = m_pModule->getSharedChannelRangeObserver()->getChannelAliases();
        for (int i = 0; i < m_GroupList.count(); i++) {
            indexList.clear();
            const QStringList grouplist = m_GroupList.at(i); // we fetch 1 list of all our grouplists
            for(const QString& group : grouplist) {
                if (channelAliases.contains(group)) // and look if all channels of that grouplist are present
                    indexList.append(channelAliases.indexOf(group));
            }
            if (grouplist.count() == indexList.count()) {
                // we found all entries of grouplist in our alias list, means group is completely present
                // so we can group now
                double maxUrValue= 0.0;
                int maxIndex = 0;
                bool groupNeedsOverloadReset = false;
                // first we search for the range with max upper range value
                for (int j = 0; j < indexList.count(); j++) {
                    const int k = indexList.at(j);
                    const QString range = m_ConfPar.getCurrentRange(k);
                    double rngUrValue = m_RangeMeasChannelList.at(k)->getUrValue(range);
                    if (maxUrValue < rngUrValue) {
                        bool allPossible = true;
                        for (int l = 0; l <indexList.count(); l++)
                            allPossible = allPossible && m_RangeMeasChannelList.at(indexList.at(l))->isPossibleRange(range);
                        // but we only take the new maximum value if all channels support this range
                        if (allPossible) {
                            maxUrValue = rngUrValue;
                            maxIndex = indexList.at(j);
                        }
                    }
                    if(requiresOverloadReset(k))
                        groupNeedsOverloadReset = true;
                }

                // then we set all channels in grouplist to that range and pass overload
                // to all channels in the group
                //
                // Background on group extra overload flags:
                // Prerequisite: Channels detect overload (of any kind) non syncronously.
                // An example what happend and made extra group handling necessary:
                // 1. Obsermatic detects I1 overload and presets I1 to max range
                // 2. Grouping presets ALL current ranges to max range
                // 3. setRange switches all current ranges to max but resets overload only
                //    for I1
                // 3. Next Obsermatic detects late I2+I3 overloads. Since ranges are already
                //    in max range no overload reset will be sent to I2+I3 so they end up in
                //    max range with overload -> range automatic stops until user resets
                //    overload manually
                QString newRange = m_ConfPar.getCurrentRange(maxIndex);
                for (int j = 0; j < indexList.count(); j++) {
                    const int k = indexList.at(j);
                    m_ConfPar.setCurrentRange(k, newRange);
                    if(groupNeedsOverloadReset) {
                        qInfo("Group overload channel %i set.", k);
                        m_groupOvlList.replace(k, true);
                    }
                }
            }
        }
    }
}


void cRangeObsermatic::setRanges(bool force)
{
    bool change = false;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we set all channels if needed
        // check if channel is in group
        QString range = m_ConfPar.getCurrentRange(i);
        cRangeMeasChannel *rangeMeasChannel = m_RangeMeasChannelList.at(i);
        if (!rangeMeasChannel->isPossibleRange(range)) { // we test whether this range is possible, otherwise we take the max. range
            range = rangeMeasChannel->getMaxRange(range);
            m_ConfPar.setCurrentRange(i, range);
        }

        if (range != m_actChannelRangeList.at(i) || force) {
            if (!change) { // signal is only set once regardingless there is more than 1 range to change
                m_pRangingSignal->setValue(QVariant(int(1)));
            }
            change = true;

            // set range
            m_MsgNrCmdList[rangeMeasChannel->setRange(range)] = setrange + i; // we must know which channel has changed for deferred notification
            m_nRangeSetPending++;
            m_actChannelRangeList.replace(i, range);

            double actUrValue = RangeMeasChannelConvenience::getUrValueRangeAct(rangeMeasChannel);
            double actRejection = RangeMeasChannelConvenience::getRejectionRangeAct(rangeMeasChannel);
            // The scaling factor is multplied with the inverse presaling value
            float scaleToDsp = (actUrValue / actRejection) * (1 / getPreScale(i));
            quint8 dspChannel = rangeMeasChannel->getDSPChannelNr();
            m_gainScaleDspVar->setValue(dspChannel, scaleToDsp);

            // we first set information of channels actual urvalue
            m_RangeActRejectionComponentList.at(i)->setValue(actUrValue);
            // we additional set information of channels actual urvalue incl. reserve
            double maxUrValue = RangeMeasChannelConvenience::getUrValueMaxRangeAct(rangeMeasChannel);
            m_RangeActOVLRejectionComponentList.at(i)->setValue(maxUrValue);

            // reset hard overload AFTER change of range.
            if (requiresOverloadReset(i) || m_groupOvlList.at(i) || force) {
                qInfo("Reset overload channel %i", i);
                m_MsgNrCmdList[rangeMeasChannel->resetStatus()] = resetstatus;
                m_hardOvlList.replace(i, false);
                m_maxOvlList.replace(i, false);
                m_groupOvlList.replace(i, false);
                m_pComponentOverloadMax->setValue(0);
            }
        }
    }

    if (change) {
        if (m_writeDspGainScaleStateMachine.isRunning())
            emit activationRepeat();
        else
            m_writeDspGainScaleStateMachine.start(); // we write all correction after each range setting
    }
    // setRanges (=this function :) is called
    // * periodically in ActionHandler
    // * here and there on special occasions
    // =>
    // * readStatus is called periodically
    // * since readStatus/analyzeStatus ignore pending status-queries, we
    //   make sure that we receive an overload status AFTER range changes
    //   finished.
    readStatus(); // get overload status
}


QList<int> cRangeObsermatic::getGroupAliasIdxListForChannel(int channelIdx)
{
    if (m_ConfPar.m_nGroupAct.m_nActive == 1) {
        const QStringList channelAliases = m_pModule->getSharedChannelRangeObserver()->getChannelAliases();
        QString channelAlias = channelAliases.at(channelIdx);
        for (const QStringList &aliasesInGroup : m_GroupList) {
            if (aliasesInGroup.contains(channelAlias)) {
                QList<int> indexlist;
                for(const QString &group : aliasesInGroup)
                    indexlist.append(channelAliases.indexOf(group));
                return indexlist;
            }
        }
    }
    return QList<int>() << channelIdx; // grouping not active or channel in no group (AUX)
}

bool cRangeObsermatic::requiresOverloadReset(int channel)
{
    // We need overload reset in case of
    // * hard overload: no explanation required - righty?
    // * soft overload: In many cases particularly when running on
    //   sources switching load with ramps our soft detection is faster
    //   and hard overloads are generated later (while we are changing
    //   range already)
    // Avoid resetting hard-overload in max range + range-automatic: It
    // would cause a infinite loop: We reset hard-overload -> hardware
    // sets it / we reset hard-overload -> hardware...
    return (m_hardOvlList.at(channel) || m_softOvlList.at(channel)) &&
           (m_ConfPar.m_nRangeAutoAct.m_nActive != 1 || !m_maxOvlList.at(channel));
}

float cRangeObsermatic::getPreScale(int channelIdx)
{
    float prescaleValue = 1;
    int groupPresacalingIdxFound = -1;
    const QStringList channelAliases = m_pModule->getSharedChannelRangeObserver()->getChannelAliases();
    if(channelIdx < channelAliases.length()){
        QString alias = channelAliases.at(channelIdx);
        for(int groupListIdx = 0; groupListIdx<m_GroupList.length(); groupListIdx++) {
            if(m_GroupList[groupListIdx].contains(alias)) {
                groupPresacalingIdxFound = groupListIdx;
            }
        }
    }

    if(groupPresacalingIdxFound < m_RangeGroupPreScalingList.length() && groupPresacalingIdxFound > -1) {
        prescaleValue = m_RangeGroupPreScalingInfo.at(groupPresacalingIdxFound)->getValue().toFloat();
    }
    return prescaleValue;
}


void cRangeObsermatic::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_readGainCorrState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cRangeObsermatic::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}


void cRangeObsermatic::readGainScale()
{
    m_gainScaleDSPMemHandle = m_dspInterface->getMemHandle("SCALEMEM");
    m_gainScaleDspVar = new cDspVar("GAINCORRECTION2",32, DSPDATA::vDspIntVar);
    m_gainScaleDSPMemHandle->addVarItem(m_gainScaleDspVar);
    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_gainScaleDSPMemHandle)] = readgainscale;
}


void cRangeObsermatic::readGainScaleDone()
{
    // our initial range set from configuration
    setRanges(true);

    // we already read all gain2corrections, set default ranges, default automatic, grouping and scaling values
    // lets now connect signals so we become alive
    const QStringList channelMNames = m_pModule->getSharedChannelRangeObserver()->getChannelMNames();
    for (int i = 0; i < channelMNames.count(); i++) {
        connect(m_RangeParameterList.at(i), &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::onNewRange);
    }

    for (int i = 0; i < m_RangeGroupPreScalingList.length(); i++) {
        connect(m_RangeGroupPreScalingList.at(i), &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::preScalingChanged);
        connect(m_RangeGroupPreScalingEnabledList.at(i), &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::preScalingChanged);
    }

    connect(m_pParRangeAutomaticOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newRangeAuto);
    connect(m_pParGroupingOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newGrouping);
    connect(m_pParOverloadOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::onVeinChangeOverload);

    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) {
        cRangeMeasChannel *rangeMeasChannel = m_RangeMeasChannelList.at(i);
        double maxUrMaxRange = RangeMeasChannelConvenience::getUrValueMaxRangeMax(rangeMeasChannel);
        m_RangeOVLRejectionComponentList.at(i)->setValue(maxUrMaxRange);
    }

    // we also have the information needed to set param validators and scpi information now
    cStringValidator *sValidator;
    const QStringList channelAliases = m_pModule->getSharedChannelRangeObserver()->getChannelAliases();
    for (int i = 0; i < channelAliases.count(); i++) {
        const QString  channelAlias = channelAliases.at(i);
        sValidator = new cStringValidator(m_RangeMeasChannelList.at(i)->getRangeListAlias());
        m_RangeParameterList.at(i)->setValidator(sValidator);
        m_ChannelRangeValidatorHash[channelMNames.at(i)] = sValidator; // systemchannelname, stringvalidator
        // we also set the channels name alias and its unit
        m_RangeParameterList.at(i)->setChannelName(channelAlias);
        m_RangeParameterList.at(i)->setScpiInfo("SENSE", QString("%1:RANGE").arg(channelAlias), SCPI::isQuery|SCPI::isCmdwP, m_RangeParameterList.at(i)->getName());
        // we want to support querying the channels ranges
        ScpiVeinComponentInfo* scpiInfo = new ScpiVeinComponentInfo("SENSE", QString("%1:RANGE:CATALOG").arg(channelAlias),
                                                                    SCPI::isQuery,
                                                                    m_RangeParameterList.at(i)->getName(),
                                                                    SCPI::isCatalog);
        m_pModule->scpiCommandList.append(scpiInfo);

        m_RangeOVLRejectionComponentList.at(i)->setChannelName(channelAlias);
        m_RangeActRejectionComponentList.at(i)->setChannelName(channelAlias);
        m_RangeActOVLRejectionComponentList.at(i)->setChannelName(channelAlias);
    }

    m_pParRangeAutomaticOnOff->setValidator(new cBoolValidator());
    m_pParRangeAutomaticOnOff->setScpiInfo("CONFIGURATION", "RNGAUTO", SCPI::isQuery|SCPI::isCmdwP, m_pParRangeAutomaticOnOff->getName());

    m_pParGroupingOnOff->setValidator(new cBoolValidator());
    m_pParGroupingOnOff->setScpiInfo("CONFIGURATION", "GROUPING", SCPI::isQuery|SCPI::isCmdwP, m_pParGroupingOnOff->getName());

    m_pParOverloadOnOff->setValidator(new cBoolValidator());
    m_pParOverloadOnOff->setScpiInfo("SENSE", "OVERLOAD", SCPI::isQuery|SCPI::isCmdwP, m_pParOverloadOnOff->getName());

    m_pParRangeAutomaticOnOff->setValue(m_ConfPar.m_nRangeAutoAct.m_nActive);
    m_pParGroupingOnOff->setValue(m_ConfPar.m_nGroupAct.m_nActive);

    m_bActive = true;
    emit activated();
}


void cRangeObsermatic::deactivationInit()
{
    m_bActive = false;
    Zera::Proxy::getInstance()->releaseConnection(m_dspClient.get());
    disconnect(m_dspInterface.get(), 0, this, 0); // we disconnect from our dsp interface
    m_dspInterface->deleteMemHandle(m_gainScaleDSPMemHandle); // and free our memory handle
    m_gainScaleDSPMemHandle = nullptr;
    m_gainScaleDspVar = nullptr; // deleteMemHandle takes care
    emit deactivationContinue();
}


void cRangeObsermatic::deactivationDone()
{
    emit deactivated();
}


void cRangeObsermatic::writeGainScale()
{
    if (m_bActive)
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_gainScaleDSPMemHandle)] = writegainScale;
}


void cRangeObsermatic::readStatus()
{
    if(m_bActive) {
        for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we read status from all channels
            cRangeMeasChannel *rangeMeasChannel = m_RangeMeasChannelList.at(i);
            m_MsgNrCmdList[rangeMeasChannel->readStatus()] = readstatus;
            m_nReadStatusPending++;
        }
    }
}


void cRangeObsermatic::analyzeStatus()
{
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we test all channels
        cRangeMeasChannel *rangeMeasChannel = m_RangeMeasChannelList.at(i);
        m_hardOvlList.replace(i, rangeMeasChannel->isHWOverload());
    }
}

void cRangeObsermatic::onNewRange(QVariant range)
{
    VfModuleParameter *pParameter = qobject_cast<VfModuleParameter*>(sender()); // get sender of updated signal
    QString rangeName = range.toString();

    // * ranges use deferred notification: new value in param 'range' / old value from getValue()
    // * SCPI requires notification to finish command
    // => on no change: just send notification
    if (rangeName == pParameter->getValue().toString()) {
        pParameter->setValue(range);
        return;
    }

    int channelIdx = m_RangeParameterList.indexOf(pParameter);
    // in case of active grouping we have to set all the ranges in that group if possible
    // so we fetch a list of index for all channels in group, in case of inactive grouping
    // the list will contain only 1 index
    const QList<int> chnGroupIndexlist = getGroupAliasIdxListForChannel(channelIdx);
    for (int channelIdxFromGroup : chnGroupIndexlist) {
        if (m_RangeMeasChannelList.at(channelIdxFromGroup)->isPossibleRange(rangeName)) {
            m_ConfPar.setCurrentRange(channelIdxFromGroup, rangeName);
            m_rangeSetManual = true;
        }
        // if we find a channel in a group that hasn't the wanted range we reset grouping!
        else {
            m_ConfPar.m_nGroupAct.m_nActive = 0;
            m_pParGroupingOnOff->setValue(0);
        }
    }
    setRanges();
    emit m_pModule->parameterChanged();
}


// called when range automatic becomes on or off
void cRangeObsermatic::newRangeAuto(QVariant rauto)
{
    bool ok;
    m_ConfPar.m_nRangeAutoAct.m_nActive = rauto.toInt(&ok);
    if (m_ConfPar.m_nRangeAutoAct.m_nActive == 1) {
        //qInfo() << "Range Automatic on";
        rangeAutomatic(); // call once if switched to automatic
        groupHandling(); // check for grouping
        setRanges();
    }
    else {
        //qInfo() << "Range Automatic off";
    }

    emit m_pModule->parameterChanged();
}


// called when grouping becomes on or off
void cRangeObsermatic::newGrouping(QVariant rgrouping)
{
    bool ok;
    m_ConfPar.m_nGroupAct.m_nActive = rgrouping.toInt(&ok);

    if ( m_ConfPar.m_nGroupAct.m_nActive == 1 ) {
        groupHandling(); // call once if switched to grouphandling
        setRanges();
    }

    emit m_pModule->parameterChanged();
}

void cRangeObsermatic::setOverloadVeinComponent(bool overloadOn)
{
    disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here
    if (overloadOn)
        m_pParOverloadOnOff->setValue(QVariant(1));
    else if (m_rangeSetManual) // only after manuell setting of range
        m_pParOverloadOnOff->setValue(QVariant(0));

    m_rangeSetManual = false;
    connect(m_pParOverloadOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::onVeinChangeOverload);
}

void cRangeObsermatic::onVeinChangeOverload(QVariant overload)
{
    disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here
    if (overload.toInt() == 0) // allthough there is a validation for this value we only accept 0 here
        // and then we force setting new ranges
        setRanges(true);

    // in each case we reset overload here
    m_pParOverloadOnOff->setValue(0);
    connect(m_pParOverloadOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::onVeinChangeOverload);
}

void cRangeObsermatic::preScalingChanged(QVariant unused)
{
    Q_UNUSED(unused)
    for(int i=0;i<m_RangeGroupPreScalingInfo.length();++i){
        float factor=1;
        if(i < m_RangeGroupPreScalingList.length()) {
            if(m_RangeGroupPreScalingEnabledList.at(i)->getValue().toBool() == true) {
                QString equation=m_RangeGroupPreScalingList.at(i)->getValue().toString();
                QStringList fac=equation.split("*");
                if(fac.length()>0){
                    float num=fac.at(0).split("/")[1].toFloat();
                    float denom=fac.at(0).split("/")[0].toFloat();
                    factor=num/denom;
                }
                // the correction factor is the inverse of the transformer ratio
                if(fac.length()>1){
                    if(fac.at(1) == "(sqrt(3))"){
                        factor = factor/sqrt(3);
                    }else if(fac.at(1) == "(1/sqrt(3))"){
                        factor = factor*sqrt(3);
                    }
                }
            }
        }

        m_RangeGroupPreScalingInfo.at(i)->setValue(factor);
        for(int channel = 0; channel<m_RangeMeasChannelList.length(); ++channel)
            m_RangeMeasChannelList.at(channel)->getChannelData()->setPreScaling(getPreScale(channel));
    }
    setRanges(true);
}

void cRangeObsermatic::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        // that we will ignore
    }
    else {
        // because rangemodulemeasprogram, adjustment and rangeobsermatic share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd) {
            case readgainscale:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(readdspgaincorrErrMsg);
                break;
            case writegainScale:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspgaincorrErrMsg);
                break;
            }
        }
    }
}

void cRangeObsermatic::catchChannelReply(quint32 msgnr)
{
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd) {
    case resetstatus: // for the moment we do nothing here
        break;
    case readstatus:
        if (m_nReadStatusPending > 0) {
            m_nReadStatusPending--;
            if (m_nReadStatusPending == 0) {
                analyzeStatus();
            }
        }
        break;

    default:
        if ((cmd >= setrange) && (cmd <= setrange +32)) {
            cmd -= setrange;
            // this is our synchronization..setValue emits notification
            m_RangeParameterList.at(cmd)->setValue(QVariant(m_actChannelRangeList.at(cmd)));
            if (m_nRangeSetPending > 0) {
                m_nRangeSetPending--;
                if (m_nRangeSetPending == 0) {
                    m_pRangingSignal->setValue(QVariant(0));
                    m_nWaitAfterRanging = 1;
                }
            }
        }
    }
}


void cRangeObsermatic::catchChannelNewRangeList()
{
    cRangeMeasChannel* mchn;

    mchn = qobject_cast<cRangeMeasChannel*>(QObject::sender());
    m_ChannelRangeValidatorHash[mchn->getMName()]->setValidator(mchn->getRangeListAlias());
    m_pModule->exportMetaData();
    setRanges(true); // after a new range list was detected, we force setting ranges because it may be that the actual range disappeared
}

}

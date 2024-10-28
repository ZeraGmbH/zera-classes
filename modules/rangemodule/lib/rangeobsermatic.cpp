#include "rangemodule.h"
#include "rangemodulemeasprogram.h"
#include "rangeobsermatic.h"
#include "rangemeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <regexvalidator.h>
#include <intvalidator.h>
#include <doublevalidator.h>
#include <boolvalidator.h>
#include <math.h>

namespace RANGEMODULE
{

cRangeObsermatic::cRangeObsermatic(cRangeModule *module, cSocket *dsprmsocket, QList<QStringList> groupList, QStringList chnlist, cObsermaticConfPar& confpar) :
    m_pModule(module),
    m_pDSPSocket(dsprmsocket),
    m_GroupList(groupList),
    m_ChannelNameList(chnlist),
    m_ConfPar(confpar)
{
    m_brangeSet = false;
    m_nWaitAfterRanging = 0;
    m_nReadStatusPending = 0;
    m_nRangeSetPending = 0;

    //  we set 0.0 as default value for all peak values in case that these values are needed before actual values really arrived

    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceRange(
        m_ChannelNameList,
        false /* just for demo COM5003 ref-session - postpone better solution now */);

    m_readGainCorrState.addTransition(this, &cRangeObsermatic::activationContinue, &m_readGainCorrDoneState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_readGainCorrState);
    m_activationMachine.addState(&m_readGainCorrDoneState);

    connect(&m_dspserverConnectState, &QState::entered, this, &cRangeObsermatic::dspserverConnect);
    connect(&m_readGainCorrState, &QState::entered, this, &cRangeObsermatic::readGainCorr);
    connect(&m_readGainCorrDoneState, &QState::entered, this, &cRangeObsermatic::readGainCorrDone);

    m_deactivationInitState.addTransition(this, &cRangeObsermatic::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_activationMachine.setInitialState(&m_dspserverConnectState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);

    connect(&m_deactivationInitState, &QState::entered, this, &cRangeObsermatic::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &cRangeObsermatic::deactivationDone);

    m_writeGainCorrState.addTransition(this, &cRangeObsermatic::activationContinue, &m_writeGainCorrDoneState);
    m_writeGainCorrState.addTransition(this, &cRangeObsermatic::activationRepeat, &m_writeGainCorrRepeatState);
    m_writeGainCorrRepeatState.addTransition(this, &cRangeObsermatic::activationContinue, &m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrRepeatState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrDoneState);
    m_writeCorrectionDSPMachine.setInitialState(&m_writeGainCorrState);
    connect(&m_writeGainCorrState, &QState::entered, this, &cRangeObsermatic::writeGainCorr);
    connect(&m_writeGainCorrDoneState, &QState::entered, this, &cRangeObsermatic::writeGainCorrDone);
}

void cRangeObsermatic::ActionHandler(QVector<float> *actualValues)
{
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


void cRangeObsermatic::generateInterface()
{
    QString s;
    QString key;

    VfModuleMetaData *pMetaData;
    VfModuleComponent *pComponent;
    VfModuleParameter *pParameter;

    for (int i = 0; i < m_ChannelNameList.count(); i++) {

        pParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              key = QString("PAR_Channel%1Range").arg(i+1),
                                              QString("Channel's range"),
                                              QVariant(s = "Unkown"),
                                              true); // we prefer deferred notification for synchronization purpose

        m_RangeParameterList.append(pParameter); // for internal use
        m_pModule->m_veinModuleParameterMap[key] = pParameter; // for modules use

        m_actChannelRangeList.append(s); // here we also fill our internal actual channel range list
        m_actChannelRangeNotifierList.append(QString(""));
        m_ChannelAliasList.append(s); // also a list for alias names
        m_RangeMeasChannelList.append(m_pModule->getMeasChannel(m_ChannelNameList.at(i)));

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

        pParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("PAR_PreScalingGroup%1").arg(i),
                                             QString("Scaling factor for group k (see configuration)"),
                                             QVariant("1/1"),
                                             false);

        pParameter->setUnit("");
        pParameter->setValidator(new cRegExValidator("^[1-9][0-9]*\\/[1-9][0-9]*(\\*\\((sqrt\\(3\\)|1\\/sqrt\\(3\\))\\))?$"));


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
        QString sep = ";";
        for (int i = 0; i < m_GroupList.count(); i++) {

          QString s = m_GroupList.at(i).join(sep);

          pMetaData = new VfModuleMetaData(QString("ChannelGroup%1").arg(i+1), QVariant(s));
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


void cRangeObsermatic::rangeObservation()
{   
    bool markOverload =false;
    qint32 nrActValues;

    nrActValues = m_RangeMeasChannelList.count();
    cRangeMeasChannel *pmChn;

    for (int i = 0; i < nrActValues; i++) { // we test all channels
        pmChn = m_RangeMeasChannelList.at(i);
        // the overload observation must consider the prescaling
        float prescalingFac=getPreScale(i);
        // for test overload we take the rms value with/without dc depending on configuration
        // and for overload condition of adc test, we take the peakvalues including dc
        bool rmsOverload = pmChn->isRMSOverload(pmChn->getRmsValue()*prescalingFac); // rms
        bool adcOverLoad = pmChn->isADCOverload(pmChn->getPeakValueWithDc()*prescalingFac); //peak
        bool hardOverLoad = m_hardOvlList.at(i);
        if ( rmsOverload || adcOverLoad || hardOverLoad) { // if any overload ?
            qInfo("Overload channel %i / Range %s: RMS %i / ADC %i / Hard %i",
                  i,
                  qPrintable(m_actChannelRangeList.at(i)),
                  rmsOverload,
                  adcOverLoad,
                  hardOverLoad);
            markOverload = true;
            // we mark each overload condition if possible (range automatic) we unmark it
            // but there was an edge on this entity

            // if an overload is recovered by rangeautomatic during running measurement

            stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(i);
            QString s = pmChn->getMaxRange(sPar.m_sPar);

            if (m_actChannelRangeList.at(i) == s) // in case ovrload was in max. range
            {
                m_maxOvlList.replace(i, true);
                m_pComponentOverloadMax->setValue(1);
            }

            sPar.m_sPar = s; // we preset the max. range here
            m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
            m_RangeOVLComponentList.at(i)->setValue(QVariant(1)); // set interface overload
            m_softOvlList.replace(i, true); //
        }
        else {
            m_RangeOVLComponentList.at(i)->setValue(QVariant(0));
            m_softOvlList.replace(i, false);
        }
    }

    disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here

    if (markOverload) {
        m_pParOverloadOnOff->setValue(QVariant(1));
    }
    else {
        if (m_brangeSet) { // only after manuell setting of range
            m_pParOverloadOnOff->setValue(QVariant(0));
        }
    }

    m_brangeSet = false;

    connect(m_pParOverloadOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newOverload);
}


void cRangeObsermatic::rangeAutomatic()
{
    if (m_ConfPar.m_nRangeAutoAct.m_nActive == 1) {
        bool unmarkOverload = true;
        cRangeMeasChannel *pmChn;

        for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we test all channels
            if (!m_maxOvlList.at(i)) { // no range automatic if there was overload in max range
                pmChn = m_RangeMeasChannelList.at(i);

                if (!m_hardOvlList.at(i)) {
                    if (!m_softOvlList.at(i)) {
                        stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(i);
                        sPar.m_sPar = pmChn->getOptRange(pmChn->getRmsValue()*getPreScale(i), sPar.m_sPar);
                        m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
                    }
                }

                else {
                    unmarkOverload = false;
                    m_MsgNrCmdList[pmChn->resetStatus()] = resetstatus;
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
        QStringList grouplist;
        QList<int> indexList;
        for (int i = 0; i < m_GroupList.count(); i++) {
            indexList.clear();
            grouplist = m_GroupList.at(i); // we fetch 1 list of all our grouplists
            for(int j = 0; j < grouplist.count(); j++) {
                if (m_ChannelAliasList.contains(grouplist.at(j))) { // and look if all channels of that grouplist are present
                    indexList.append(m_ChannelAliasList.indexOf(grouplist.at(j)));
                }
            }
            if (grouplist.count() == indexList.count()) {
                // we found all entries of grouplist in our alias list, means group is completely present
                // so we can group now
                double maxUrValue= 0.0;
                double rngUrValue;
                int maxIndex = 0;

                bool groupNeedsOverloadReset = false;
                // first we search for the range with max upper range value
                for (int j = 0; j < indexList.count(); j++) {
                    int k = indexList.at(j);
                    rngUrValue = m_RangeMeasChannelList.at(k)->getUrValue(m_ConfPar.m_senseChannelRangeParameter.at(k).m_sPar);
                    if (maxUrValue < rngUrValue) {
                        bool allPossible =true;
                        for (int l = 0; l <indexList.count(); l++) {
                            allPossible = allPossible && m_RangeMeasChannelList.at(indexList.at(l))->isPossibleRange(m_ConfPar.m_senseChannelRangeParameter.at(k).m_sPar);
                        }
                        // but we only take the new maximum value if all channels support this range
                        if (allPossible) {
                            maxUrValue = rngUrValue;
                            maxIndex = indexList.at(j); //
                        }
                    }
                    if(requiresOverloadReset(k)) {
                        groupNeedsOverloadReset = true;
                    }
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
                QString newRange = m_ConfPar.m_senseChannelRangeParameter.at(maxIndex).m_sPar;
                for (int j = 0; j < indexList.count(); j++) {
                    int k = indexList.at(j);
                    stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(k);
                    sPar.m_sPar = newRange;
                    m_ConfPar.m_senseChannelRangeParameter.replace(k, sPar);
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
    cRangeMeasChannel *pmChn;
    QString s;
    quint8 chn;
    bool change;

    change = false;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we set all channels if needed


        // check if channel is in group
        float preScalingFactor=1;
        preScalingFactor=getPreScale(i);
        s = m_ConfPar.m_senseChannelRangeParameter.at(i).m_sPar;
        pmChn = m_RangeMeasChannelList.at(i);
        if (! pmChn->isPossibleRange(s)) { // we test whether this range is possible, otherwise we take the max. range
            stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(i);
            s = pmChn->getMaxRange(sPar.m_sPar);
            sPar.m_sPar = s;
            m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
        }

        if ( s != m_actChannelRangeList.at(i) || force) {
            if (!change) { // signal is only set once regardingless there is more than 1 range to change
                m_pRangingSignal->setValue(QVariant(int(1)));
            }
            change = true;

            // set range
            m_MsgNrCmdList[pmChn->setRange(s)] = setrange + i; // we must know which channel has changed for deferred notification
            m_nRangeSetPending++;
            m_actChannelRangeList.replace(i, s);

            // we set the scaling factor here
            chn = pmChn->getDSPChannelNr();

            // The scaling factor is multplied with the inverse presaling value
            m_pfScale[chn] = (pmChn->getUrValue() / pmChn->getRejection()) * (1/preScalingFactor);

            // we first set information of channels actual urvalue
            m_RangeActRejectionComponentList.at(i)->setValue(pmChn->getUrValue());
            // we additional set information of channels actual urvalue incl. reserve
            m_RangeActOVLRejectionComponentList.at(i)->setValue(pmChn->getRangeUrvalueMax()); // we additional set information of channels actual urvalue incl. reserve

            // reset hard overload AFTER change of range.
            if (requiresOverloadReset(i) || m_groupOvlList.at(i) || force) {
                qInfo("Reset overload channel %i", i);
                m_MsgNrCmdList[pmChn->resetStatus()] = resetstatus;
                m_hardOvlList.replace(i, false);
                m_maxOvlList.replace(i, false);
                m_groupOvlList.replace(i, false);
                m_pComponentOverloadMax->setValue(0);
            }
        }

        else {
            // the parameter delegate had memorized that there will be a deferred notification
            // so we have to give this even in case nothing has changed. otherwise there will
            // remain pending synchronisation marks...but we must remember when we have sent notification
            // to ensure that we only send it once after change

            if (m_actChannelRangeNotifierList.at(i) != m_actChannelRangeList.at(i))
            {
                m_RangeParameterList.at(i)->setValue(QVariant(m_actChannelRangeList.at(i)));
                m_actChannelRangeNotifierList.replace(i, (m_actChannelRangeList.at(i)));
            }
        }
    }

    if (change) {
        if (m_writeCorrectionDSPMachine.isRunning()) {
            emit activationRepeat();
        }
        else {
            m_writeCorrectionDSPMachine.start(); // we write all correction after each range setting
        }
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


QList<int> cRangeObsermatic::getGroupIndexList(int index)
{
    QList<int> indexlist;
    if (m_ConfPar.m_nGroupAct.m_nActive == 1) {
        QString s = m_ChannelAliasList.at(index); // we search for this channel alias
        QStringList grouplist;
        for (int i = 0; i < m_GroupList.count(); i++)
        {
            grouplist = m_GroupList.at(i); // we fetch 1 list of all our grouplists
            if (grouplist.contains(s)) // if this grouplist contains the searched item
            {
                for(int j = 0; j < grouplist.count(); j++) // we build our list of channel index
                    indexlist.append(m_ChannelAliasList.indexOf(grouplist.at(j)));
                break;
            }
        }
        if (indexlist.isEmpty())
            // if we have a channel that is not included in a grouping list
            indexlist.append(index);
    }
    else {
        indexlist.append(index); // we return 1 index at least
    }

    return indexlist;
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
    return (m_hardOvlList.at(channel) || m_softOvlList.at(channel)) && (m_ConfPar.m_nRangeAutoAct.m_nActive != 1 || !m_maxOvlList.at(channel));
}

float cRangeObsermatic::getPreScale(int p_idx)
{
    float retVal=1;
    int group=-1;
    if(p_idx < m_ChannelAliasList.length()){
        QString alias=m_ChannelAliasList.at(p_idx);
        for(int k = 0; k < m_GroupList.length();k++){
            if(m_GroupList[k].contains(alias)){
                group=k;
            }
        }
    }


    if(group < m_RangeGroupPreScalingList.length() && group > -1) {
        retVal=m_RangeGroupPreScalingInfo.at(group)->getValue().toFloat();
    }
   // qInfo("Pre Scaling is %f", retVal);
    return retVal;
}


void cRangeObsermatic::dspserverConnect()
{
    // the alias list is correctly filled when activating range obsermatic
    // the module has first activated the channels before activating rangeobsermatic
    for (int i = 0; i < m_ChannelNameList.count(); i++) {
        m_ChannelAliasList.replace(i, m_RangeMeasChannelList.at(i)->getAlias());
    }

    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pDSPSocket->m_sIP,
                                                                 m_pDSPSocket->m_nPort,
                                                                 m_pModule->getTcpWorkerFactory());
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_readGainCorrState);
    connect(m_dspInterface.get(), &Zera::cDSPInterface::serverAnswer, this, &cRangeObsermatic::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}


void cRangeObsermatic::readGainCorr()
{
    // qInfo() << "readGainCorr";
    m_pGainCorrection2DSP = m_dspInterface->getMemHandle("SCALEMEM");
    m_pGainCorrection2DSP->addVarItem( new cDspVar("GAINCORRECTION2",32, DSPDATA::vDspIntVar));
    m_pfScale =  m_pGainCorrection2DSP->data("GAINCORRECTION2");
    m_MsgNrCmdList[m_dspInterface->dspMemoryRead(m_pGainCorrection2DSP)] = readgain2corr;
}


void cRangeObsermatic::readGainCorrDone()
{
    // our initial range set from configuration
    setRanges(true);

    // we already read all gain2corrections, set default ranges, default automatic, grouping and scaling values
    // lets now connect signals so we become alive
    for (int i = 0; i < m_ChannelNameList.count(); i++) {
        connect(m_RangeParameterList.at(i), &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newRange);
    }

    for (int i = 0; i < m_RangeGroupPreScalingList.length(); i++) {
        connect(m_RangeGroupPreScalingList.at(i), &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::preScalingChanged);
        connect(m_RangeGroupPreScalingEnabledList.at(i), &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::preScalingChanged);
    }

    connect(m_pParRangeAutomaticOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newRangeAuto);
    connect(m_pParGroupingOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newGrouping);
    connect(m_pParOverloadOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newOverload);

    cRangeMeasChannel *pmChn;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) {
        pmChn = m_RangeMeasChannelList.at(i);
        m_RangeOVLRejectionComponentList.at(i)->setValue(pmChn->getMaxRangeUrvalueMax());
    }

    // we also have the information needed to set param validators and scpi information now

    cStringValidator *sValidator;
    cSCPIInfo *scpiInfo;
    for (int i = 0; i < m_ChannelNameList.count(); i++) {
        QString s1, s2;
        sValidator = new cStringValidator(m_RangeMeasChannelList.at(i)->getRangeListAlias());
        m_RangeParameterList.at(i)->setValidator(sValidator);
        m_ChannelRangeValidatorHash[m_ChannelNameList.at(i)] = sValidator; // systemchannelname, stringvalidator
        // we also set the channels name alias and its unit
        m_RangeParameterList.at(i)->setChannelName(s1 = m_ChannelAliasList.at(i));

        scpiInfo = new cSCPIInfo("SENSE", QString("%1:RANGE").arg(m_ChannelAliasList.at(i)), "10", m_RangeParameterList.at(i)->getName(), "0", s2);
        m_RangeParameterList.at(i)->setSCPIInfo(scpiInfo);

        // we want to support querying the channels ranges
        scpiInfo = new cSCPIInfo("SENSE", QString("%1:RANGE:CATALOG").arg(m_ChannelAliasList.at(i)), "2", m_RangeParameterList.at(i)->getName(), "1", "");
        m_pModule->scpiCommandList.append(scpiInfo);

        m_RangeOVLRejectionComponentList.at(i)->setChannelName(s1);
        m_RangeOVLRejectionComponentList.at(i)->setUnit(s2);

        m_RangeActRejectionComponentList.at(i)->setChannelName(s1);
        m_RangeActRejectionComponentList.at(i)->setUnit(s2);

        m_RangeActOVLRejectionComponentList.at(i)->setChannelName(s1);
        m_RangeActOVLRejectionComponentList.at(i)->setUnit(s2);
    }

    scpiInfo = new cSCPIInfo("CONFIGURATION", "RNGAUTO", "10", m_pParRangeAutomaticOnOff->getName(), "0", "");
    m_pParRangeAutomaticOnOff->setValidator(new cBoolValidator());
    m_pParRangeAutomaticOnOff->setSCPIInfo(scpiInfo);

    scpiInfo = new cSCPIInfo("CONFIGURATION", "GROUPING", "10", m_pParGroupingOnOff->getName(), "0", "");
    m_pParGroupingOnOff->setValidator(new cBoolValidator());
    m_pParGroupingOnOff->setSCPIInfo(scpiInfo);

    scpiInfo = new cSCPIInfo("SENSE", "OVERLOAD", "10", m_pParOverloadOnOff->getName(), "0", "");
    m_pParOverloadOnOff->setValidator(new cBoolValidator());
    m_pParOverloadOnOff->setSCPIInfo(scpiInfo);

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
    m_dspInterface->deleteMemHandle(m_pGainCorrection2DSP); // and free our memory handle
    emit deactivationContinue();
}


void cRangeObsermatic::deactivationDone()
{
    emit deactivated();
}


void cRangeObsermatic::writeGainCorr()
{
    // qInfo() << "writeGainCorr";
    if (m_bActive) {
        m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pGainCorrection2DSP)] = writegain2corr;
    }
}


void cRangeObsermatic::writeGainCorrDone()
{
    // qInfo() << "writeGainCorrDone";
}


void cRangeObsermatic::readStatus()
{
    cRangeMeasChannel *pmChn;
    if(m_bActive) {
        for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we read status from all channels
            pmChn = m_RangeMeasChannelList.at(i);
            m_MsgNrCmdList[pmChn->readStatus()] = readstatus;
            m_nReadStatusPending++;
        }
    }
}


void cRangeObsermatic::analyzeStatus()
{
    cRangeMeasChannel *pmChn;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) { // we test all channels
        pmChn = m_RangeMeasChannelList.at(i);
        m_hardOvlList.replace(i, pmChn->isHWOverload());
    }
}


// called when new range is selected
void cRangeObsermatic::newRange(QVariant range)
{
    VfModuleParameter *pParameter = qobject_cast<VfModuleParameter*>(sender()); // get sender of updated signal
    int index = m_RangeParameterList.indexOf(pParameter); // which channel is it

    QString s;
    s = range.toString();

    QList<int> chnIndexlist = getGroupIndexList(index);
    // in case of active grouping we have to set all the ranges in that group if possible
    // so we fetch a list of index for all channels in group ,in case of inactive grouping
    // the list will contain only 1 index
    // if we find 1 channel in a group that hasn't the wanted range we reset grouping !
    // let's first test if all channels have the wanted range

    for (int i = 0; i < chnIndexlist.count(); i++) {
        index = chnIndexlist.at(i);
        if (m_RangeMeasChannelList.at(index)->isPossibleRange(s)) {
            stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(index);
            sPar.m_sPar = s;
            m_ConfPar.m_senseChannelRangeParameter.replace(index, sPar);
            m_brangeSet = true;
            m_actChannelRangeNotifierList.replace(index,QString("")); // this will assure that a notification will be sent after setRanges()
        }
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

// called when overload is reset
void cRangeObsermatic::newOverload(QVariant overload)
{
    disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here

    bool ok;
    if (overload.toInt(&ok) == 0) { // allthough there is a validation for this value we only accept 0 here
        // and then we force setting new ranges
        setRanges(true);
    }

    // in each case we reset overload here
    m_pParOverloadOnOff->setValue(0);

    connect(m_pParOverloadOnOff, &VfModuleParameter::sigValueChanged, this, &cRangeObsermatic::newOverload);
}

void cRangeObsermatic::preScalingChanged(QVariant unused)
{
    Q_UNUSED(unused)
    for(int i=0;i<m_RangeGroupPreScalingInfo.length();++i){
        float factor=1;
        if(i < m_RangeGroupPreScalingList.length())
        {
            if(m_RangeGroupPreScalingEnabledList.at(i)->getValue().toBool() == true){
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
        for(int i = 0; i< m_RangeMeasChannelList.length();++i){
            m_RangeMeasChannelList.at(i)->setPreScaling(getPreScale(i));
        }
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
            case readgain2corr:
                if (reply == ack) {
                    emit activationContinue();
                }
                else {
                    emit errMsg((tr(readdspgaincorrErrMsg)));
                    emit activationError();
                }
                break;
            case writegain2corr:
                if (reply == ack) {
                    emit activationContinue();
                }
                else {
                    emit errMsg((tr(writedspgaincorrErrMsg)));
                    // emit activationError();
                    emit executionError(); // we also emit exec error because
                }
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
            m_actChannelRangeNotifierList.replace(cmd, (m_actChannelRangeList.at(cmd)));
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
    m_ChannelRangeValidatorHash[mchn->getName()]->setValidator(mchn->getRangeListAlias());
    m_pModule->exportMetaData();
    setRanges(true); // after a new range list was detected, we force setting ranges because it may be that the actual range disappeared
}

}

#include <QString>
#include <QVector>
#include <QPoint>
#include <QJsonObject>
#include <QJsonArray>
#include <math.h>
#include <veinmodulemetadata.h>
#include <veinmodulecomponent.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <dspinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <stringvalidator.h>
#include <boolvalidator.h>
#include <scpiinfo.h>

#include "debug.h"
#include "errormessages.h"
#include "rangemodule.h"
#include "rangeobsermatic.h"
#include "rangemeaschannel.h"



namespace RANGEMODULE
{

cRangeObsermatic::cRangeObsermatic(cRangeModule *module, Zera::Proxy::cProxy* proxy, cSocket *dsprmsocket, QList<QStringList> groupList, QStringList chnlist, cObsermaticConfPar& confpar)
    :m_pModule(module), m_pProxy(proxy), m_pDSPSocket(dsprmsocket), m_GroupList(groupList), m_ChannelNameList(chnlist), m_ConfPar(confpar)
{
    m_brangeSet =false;
    m_nWaitAfterRanging = 0;
    m_nRangeSetPending = 0;

    //  we set 0.0 as default value for all peak values in case that these values are needed before actual values really arrived
    for (int i = 0; i < m_ChannelNameList.count(); i++)
        m_ActualValues.append(0.0);

    m_pDSPInterFace = new Zera::Server::cDSPInterface();

    m_readGainCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readGainCorrDoneState);
    m_activationMachine.addState(&m_dspserverConnectState);
    m_activationMachine.addState(&m_readGainCorrState);
    m_activationMachine.addState(&m_readGainCorrDoneState);
    m_activationMachine.setInitialState(&m_dspserverConnectState);
    connect(&m_dspserverConnectState, SIGNAL(entered()), SLOT(dspserverConnect()));
    connect(&m_readGainCorrState, SIGNAL(entered()), SLOT(readGainCorr()));
    connect(&m_readGainCorrDoneState, SIGNAL(entered()), SLOT(readGainCorrDone()));

    m_deactivationInitState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, SIGNAL(entered()), SLOT(deactivationInit()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));

    m_writeGainCorrState.addTransition(this, SIGNAL(activationContinue()), &m_writeGainCorrDoneState);
    m_writeGainCorrState.addTransition(this, SIGNAL(activationRepeat()), &m_writeGainCorrRepeatState);
    m_writeGainCorrRepeatState.addTransition(this, SIGNAL(activationContinue()), &m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrRepeatState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrDoneState);
    m_writeCorrectionDSPMachine.setInitialState(&m_writeGainCorrState);
    connect(&m_writeGainCorrState, SIGNAL(entered()), SLOT(writeGainCorr()));
    connect(&m_writeGainCorrDoneState, SIGNAL(entered()), SLOT(writeGainCorrDone()));

    m_readStatusState.addTransition(this, SIGNAL(readStatusContinue()), &m_analyzeStatusState);
    m_readStatusMachine.addState(&m_readStatusState);
    m_readStatusMachine.addState(&m_analyzeStatusState);
    m_readStatusMachine.setInitialState(&m_readStatusState);
    connect(&m_readStatusState, SIGNAL(entered()), SLOT(readStatus()));
    connect(&m_analyzeStatusState, SIGNAL(entered()), SLOT(analyzeStatus()));

}


cRangeObsermatic::~cRangeObsermatic()
{
    delete m_pDSPInterFace;
}


void cRangeObsermatic::ActionHandler(QVector<float> *actualValues)
{
    if (m_nRangeSetPending == 0)
    {
        if (m_nWaitAfterRanging > 0)
        {
            m_nWaitAfterRanging--;
        }
        else
        {
            m_ActualValues = *actualValues;
            // qDebug() << "range obsermatic new actual values";

#ifdef DEBUG
            qDebug() << QString("PEAK %1 ; %2 ; %3 ;").arg(m_ActualValues[0]).arg(m_ActualValues[1]).arg(m_ActualValues[2])
                     << QString("%1 ; %2 ; %3").arg(m_ActualValues[3]).arg(m_ActualValues[4]).arg(m_ActualValues[5]);
#endif
            // qDebug() << QString("RMS %1 ; %2 ; %3").arg(m_ActualValues[6]).arg(m_ActualValues[7]).arg(m_ActualValues[8]);

            rangeObservation(); // first we test for overload conditions
            rangeAutomatic(); // let rangeautomatic do its job
            groupHandling(); // and look for grouping channels if necessary
            setRanges(); // set the new ranges now
            if (!m_readStatusMachine.isRunning()) // we only start if not running
                m_readStatusMachine.start();
        }
    }
}


void cRangeObsermatic::generateInterface()
{
    QString s;
    QString key;

    cVeinModuleMetaData *pMetaData;
    cVeinModuleComponent *pComponent;
    cVeinModuleParameter *pParameter;

    for (int i = 0; i < m_ChannelNameList.count(); i++)
    {

        pParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("PAR_Channel%1Range").arg(i+1),
                                              QString("Component for reading and setting the channels range"),
                                              QVariant(s = "Unkown"),
                                              true); // we prefer deferred notification for synchronization purpose

        m_RangeParameterList.append(pParameter); // for internal use
        m_pModule->veinModuleParameterHash[key] = pParameter; // for modules use

        m_actChannelRangeList.append(s); // here we also fill our internal actual channel range list
        m_actChannelRangeNotifierList.append(QString(""));
        m_ChannelAliasList.append(s); // also a list for alias names
        m_RangeMeasChannelList.append(m_pModule->getMeasChannel(m_ChannelNameList.at(i)));

        pComponent = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              QString("SIG_Channel%1OVL").arg(i+1),
                                              QString("Component forwards the channels overload status"),
                                              QVariant(int(0)) );

        m_RangeOVLComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        pComponent = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              QString("INF_Channel%1OVLREJ").arg(i+1),
                                              QString("Component forwards the channels maximum range possible peak rejection"),
                                              QVariant(double(0.0)) );

        m_RangeOVLRejectionComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        pComponent = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              QString("INF_Channel%1ActREJ").arg(i+1),
                                              QString("Component forwards the channels actual rejection"),
                                              QVariant(double(0.0)) );

        m_RangeActRejectionComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        pComponent = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              QString("INF_Channel%1ActOVLREJ").arg(i+1),
                                              QString("Component forwards the channels actual range possible peak rejection"),
                                              QVariant(double(0.0)) );

        m_RangeActOVLRejectionComponentList.append(pComponent);
        m_pModule->veinModuleComponentList.append(pComponent);

        m_softOvlList.append(false);
        m_hardOvlList.append(false);
        m_maxOvlList.append(false);
    }

    if (m_GroupList.count() > 0)
    {
        QString sep = ";";
        for (int i = 0; i < m_GroupList.count(); i++)
        {

          QString s = m_GroupList.at(i).join(sep);

          pMetaData = new cVeinModuleMetaData(QString("ChannelGroup%1").arg(i+1), QVariant(s));
          m_pModule->veinModuleMetaDataList.append(pMetaData); // only for module use
        }
    }

    if (m_ConfPar.m_bRangeAuto)
    {
        m_pParRangeAutomaticOnOff = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                             QString("PAR_RangeAutomatic"),
                                                             QString("Component for switching on/off the range automatic"),
                                                             QVariant(0));

        m_pModule->veinModuleParameterHash["PAR_RangeAutomatic"] = m_pParRangeAutomaticOnOff; // for modules use
    }

    if (m_ConfPar.m_bGrouping)
    {
        m_pParGroupingOnOff = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       QString("PAR_ChannelGrouping"),
                                                       QString("Component for switching on/off channel grouping"),
                                                       QVariant(0));

        m_pModule->veinModuleParameterHash["PAR_ChannelGrouping"] = m_pParGroupingOnOff; // for modules use
    }

    if (m_ConfPar.m_bOverload)
    {
        m_pParOverloadOnOff = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       QString("PAR_Overload"),
                                                       QString("Component for reading and resetting overload"),
                                                       QVariant(0));

        m_pModule->veinModuleParameterHash["PAR_Overload"] = m_pParOverloadOnOff; // for modules use

        m_pComponentOverloadMax = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                           QString("INF_OverloadMax"),
                                                           QString("Component signals maximum range overload condition"),
                                                           QVariant(0));

        m_pModule->veinModuleComponentList.append(m_pComponentOverloadMax);
    }

    m_pRangingSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Ranging"),
                                                QString("Component forwards information that ranges are changing"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pRangingSignal);
}


void cRangeObsermatic::deleteInterface()
{
}


void cRangeObsermatic::rangeObservation()
{   
    bool markOverload =false;
    qint32 nrActValues;

    nrActValues = m_RangeMeasChannelList.count();
    cRangeMeasChannel *pmChn;

    for (int i = 0; i < nrActValues; i++) // we test all channels
    {
        pmChn = m_RangeMeasChannelList.at(i);

        // for test overload we take the rms value with/without dc depending on configuration
        // and for overload condition of adc test, we take the peakvalues including dc
        if ( (pmChn->isRMSOverload(m_ActualValues[nrActValues+i])) || (pmChn->isADCOverload(m_ActualValues[2 * nrActValues + 1 + i])) || m_hardOvlList.at(i)) // if any overload ?
        {
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
        else
        {
            m_RangeOVLComponentList.at(i)->setValue(QVariant(0));
            m_softOvlList.replace(i, false);
        }
    }

    disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here

    if (markOverload)
        m_pParOverloadOnOff->setValue(QVariant(1));
    else
        if (m_brangeSet) // only after manuell setting of range
            m_pParOverloadOnOff->setValue(QVariant(0));

    m_brangeSet = false;

    connect(m_pParOverloadOnOff, SIGNAL(sigValueChanged(QVariant)), SLOT(newOverload(QVariant)));
}


void cRangeObsermatic::rangeAutomatic()
{
    if (m_bRangeAutomatic)
    {
        bool unmarkOverload = true;
        cRangeMeasChannel *pmChn;

        for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we test all channels
        {
            if (!m_maxOvlList.at(i)) // no range automatic if there was overload in max range
            {
                pmChn = m_RangeMeasChannelList.at(i);

                if (!m_hardOvlList.at(i))
                {
                    if (!m_softOvlList.at(i))
                    {
                        stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(i);
                        sPar.m_sPar = pmChn->getOptRange(m_ActualValues[i], sPar.m_sPar);
                        m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
                    }
                }

                else
                {
                    unmarkOverload = false;
                    m_MsgNrCmdList[pmChn->resetStatus()] = resetstatus;
                }
            }

            else
                unmarkOverload = false;
        }

        if (unmarkOverload) // if we could recover all overloads
            m_pParOverloadOnOff->setValue(QVariant(int(0)));
    }
}


void cRangeObsermatic::groupHandling()
{
    if (m_bGrouping)
    {
        QStringList grouplist;
        QList<int> indexList;
        for (int i = 0; i < m_GroupList.count(); i++)
        {
            indexList.clear();
            grouplist = m_GroupList.at(i); // we fetch 1 list of all our grouplists
            for(int j = 0; j < grouplist.count(); j++)
                if (m_ChannelAliasList.contains(grouplist.at(j))) // and look if all channels of that grouplist are present
                    indexList.append(m_ChannelAliasList.indexOf(grouplist.at(j)));
            if (grouplist.count() == indexList.count())
            {
                // we found all entries of grouplist in our alias list, means group is completely present
                // so we can group now
                double maxUrValue= 0.0;
                double rngUrValue;
                int maxIndex = 0;

                // first we search for the range with max upper range value
                for (int j = 0; j < indexList.count(); j++)
                {
                    int k = indexList.at(j);
                    rngUrValue = m_RangeMeasChannelList.at(k)->getUrValue(m_ConfPar.m_senseChannelRangeParameter.at(k).m_sPar);
                    if (maxUrValue < rngUrValue)
                    {
                        bool allPossible =true;
                        for (int l = 0; l <indexList.count(); l++)
                            allPossible = allPossible && m_RangeMeasChannelList.at(indexList.at(l))->isPossibleRange(m_ConfPar.m_senseChannelRangeParameter.at(k).m_sPar);
                        // but we only take the new maximum value if all channels support this range
                        if (allPossible)
                        {
                            maxUrValue = rngUrValue;
                            maxIndex = indexList.at(j); //
                        }
                    }
                }

                // then we set all channels in grouplist to that range
                QString newRange = m_ConfPar.m_senseChannelRangeParameter.at(maxIndex).m_sPar;
                for (int j = 0; j < indexList.count(); j++)
                {
                    int k = indexList.at(j);
                    stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(k);
                    sPar.m_sPar = newRange;
                    m_ConfPar.m_senseChannelRangeParameter.replace(k, sPar);
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
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we set all channels if needed
    {
        s = m_ConfPar.m_senseChannelRangeParameter.at(i).m_sPar;
        pmChn = m_RangeMeasChannelList.at(i);
        if (! pmChn->isPossibleRange(s)) // we test whether this range is possible, otherwise we take the max. range
        {
            stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(i);
            s = pmChn->getMaxRange(sPar.m_sPar);
            sPar.m_sPar = s;
            m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
        }

        if ( s != m_actChannelRangeList.at(i) || force)
        {
            if (!change) // signal is only set once regardingless there is more than 1 range to change
            {
#ifdef DEBUG
                qDebug() << "SIG_RANGING = 1";
#endif
                m_pRangingSignal->setValue(QVariant(int(1)));
            }

            change = true;

            // if we have an overload condition in channel we reset it before we set the new range
            // but we don't do this if we are in range automatic and have an overload in max. range
            if (m_hardOvlList.at(i) && !(m_maxOvlList.at(i) && m_bRangeAutomatic))
            {
                m_MsgNrCmdList[pmChn->resetStatus()] = resetstatus;
                m_hardOvlList.replace(i, false);
                m_maxOvlList.replace(i, false);
            }

            m_MsgNrCmdList[pmChn->setRange(s)] = setrange + i; // we must know which channel has changed for deferred notification
            m_nRangeSetPending++;
            m_actChannelRangeList.replace(i, s);

            // we set the scaling factor here
            chn = pmChn->getDSPChannelNr();
            m_pfScale[chn] = pmChn->getUrValue() / pmChn->getRejection();

            // we first set information of channels actual urvalue
            m_RangeActRejectionComponentList.at(i)->setValue(pmChn->getUrValue());
            // we additional set information of channels actual urvalue incl. reserve
            m_RangeActOVLRejectionComponentList.at(i)->setValue(pmChn->getRangeUrvalueMax()); // we additional set information of channels actual urvalue incl. reserve

#ifdef DEBUG
            qDebug() << QString("setRange Ch%1; %2; Scale=%3").arg(chn).arg(s).arg(m_pfScale[chn]);
#endif
        }

        else
        {
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

    if (change)
        if (m_writeCorrectionDSPMachine.isRunning())
            emit activationRepeat();
        else
            m_writeCorrectionDSPMachine.start(); // we write all correction after each range setting
}


QList<int> cRangeObsermatic::getGroupIndexList(int index)
{
    QList<int> indexlist;
    QString s = m_ChannelAliasList.at(index); // we search for this channel alias
    if (m_bGrouping)
    {
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
    else
        indexlist.append(index); // we return 1 index at least

    return indexlist;
}


void cRangeObsermatic::dspserverConnect()
{
    // the alias list is correctly filled when activating range obsermatic
    // the module has first activated the channels before activating rangeobsermatic
    for (int i = 0; i < m_ChannelNameList.count(); i++)
        m_ChannelAliasList.replace(i, m_RangeMeasChannelList.at(i)->getAlias());

    m_pDspClient = m_pProxy->getConnection(m_pDSPSocket->m_sIP, m_pDSPSocket->m_nPort);
    m_pDSPInterFace->setClient(m_pDspClient);
    m_dspserverConnectState.addTransition(m_pDspClient, SIGNAL(connected()), &m_readGainCorrState);
    connect(m_pDSPInterFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pDspClient);
}


void cRangeObsermatic::readGainCorr()
{
    // qDebug() << "readGainCorr";
    m_pGainCorrection2DSP = m_pDSPInterFace->getMemHandle("SCALEMEM");
    m_pGainCorrection2DSP->addVarItem( new cDspVar("GAINCORRECTION2",32, DSPDATA::vDspIntVar));
    m_pfScale =  m_pDSPInterFace->data(m_pGainCorrection2DSP, "GAINCORRECTION2");
    m_MsgNrCmdList[m_pDSPInterFace->dspMemoryRead(m_pGainCorrection2DSP)] = readgain2corr;
}


void cRangeObsermatic::readGainCorrDone()
{
    newRangeAuto(m_ConfPar.m_nRangeAutoAct.m_nActive);
    newGrouping(m_ConfPar.m_nGroupAct.m_nActive);
    setRanges(); // so we set our scaling factors if not already done

    // we already read all gain2corrections, set default ranges, default automatic, grouping and scaling values
    // lets now connect signals so we become alive
    for (int i = 0; i < m_ChannelNameList.count(); i++)
        connect(m_RangeParameterList.at(i), SIGNAL(sigValueChanged(QVariant)), SLOT(newRange(QVariant)));

    if (m_ConfPar.m_bRangeAuto)
        connect(m_pParRangeAutomaticOnOff, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newRangeAuto(QVariant)));
    if (m_ConfPar.m_bGrouping)
        connect(m_pParGroupingOnOff, SIGNAL(sigValueChanged(QVariant)), SLOT(newGrouping(QVariant)));
    if (m_ConfPar.m_bOverload)
        connect(m_pParOverloadOnOff, SIGNAL(sigValueChanged(QVariant)), SLOT(newOverload(QVariant)));

    cRangeMeasChannel *pmChn;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++)
    {
        pmChn = m_RangeMeasChannelList.at(i);
        m_RangeOVLRejectionComponentList.at(i)->setValue(pmChn->getMaxRangeUrvalueMax());
    }

    // we also have the information needed to set param validators and scpi information now

    cStringValidator *sValidator;
    cSCPIInfo *scpiInfo;
    for (int i = 0; i < m_ChannelNameList.count(); i++)
    {
        QString s1, s2;
        sValidator = new cStringValidator(m_RangeMeasChannelList.at(i)->getRangeListAlias());
        m_RangeParameterList.at(i)->setValidator(sValidator);
        m_ChannelRangeValidatorHash[m_ChannelNameList.at(i)] = sValidator; // systemchannelname, stringvalidator
        // we also set the channels name alias and its unit
        m_RangeParameterList.at(i)->setChannelName(s1 = m_ChannelAliasList.at(i));
        m_RangeParameterList.at(i)->setUnit(s2 = m_RangeMeasChannelList.at(i)->getUnit());

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

    if (m_ConfPar.m_bRangeAuto)
    {
        scpiInfo = new cSCPIInfo("CONFIGURATION", "RNGAUTO", "10", m_pParRangeAutomaticOnOff->getName(), "0", "");
        m_pParRangeAutomaticOnOff->setValidator(new cBoolValidator());
        m_pParRangeAutomaticOnOff->setSCPIInfo(scpiInfo);
    }

    if (m_ConfPar.m_bGrouping)
    {
        scpiInfo = new cSCPIInfo("CONFIGURATION", "GROUPING", "10", m_pParGroupingOnOff->getName(), "0", "");
        m_pParGroupingOnOff->setValidator(new cBoolValidator());
        m_pParGroupingOnOff->setSCPIInfo(scpiInfo);
    }

    if (m_ConfPar.m_bOverload)
    {
        scpiInfo = new cSCPIInfo("SENSE", "OVERLOAD", "10", m_pParOverloadOnOff->getName(), "0", "");
        m_pParOverloadOnOff->setValidator(new cBoolValidator());
        m_pParOverloadOnOff->setSCPIInfo(scpiInfo);
    }

    m_pParRangeAutomaticOnOff->setValue(m_ConfPar.m_nRangeAutoAct.m_nActive);
    m_pParGroupingOnOff->setValue(m_ConfPar.m_nGroupAct.m_nActive);

    m_bActive = true;
    emit activated();
}


void cRangeObsermatic::deactivationInit()
{
    m_bActive = false;
    m_pProxy->releaseConnection(m_pDspClient);
    disconnect(m_pDSPInterFace, 0, this, 0); // we disconnect from our dsp interface
    m_pDSPInterFace->deleteMemHandle(m_pGainCorrection2DSP); // and free our memory handle
    emit deactivationContinue();
}


void cRangeObsermatic::deactivationDone()
{
    emit deactivated();
}


void cRangeObsermatic::writeGainCorr()
{
    // qDebug() << "writeGainCorr";
    if (m_bActive)
        m_MsgNrCmdList[m_pDSPInterFace->dspMemoryWrite(m_pGainCorrection2DSP)] = writegain2corr;
}


void cRangeObsermatic::writeGainCorrDone()
{
    // qDebug() << "writeGainCorrDone";
}


void cRangeObsermatic::readStatus()
{
    cRangeMeasChannel *pmChn;

    if (m_bActive)
    {
        m_nReadStatusPending = 0;
        for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we read status from all channels
        {
            pmChn = m_RangeMeasChannelList.at(i);
            m_MsgNrCmdList[pmChn->readStatus()] = readstatus;
            m_nReadStatusPending++;
        }
    }
}


void cRangeObsermatic::analyzeStatus()
{
    cRangeMeasChannel *pmChn;

    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we test all channels
    {
        pmChn = m_RangeMeasChannelList.at(i);
        m_hardOvlList.replace(i, pmChn->isHWOverload());
    }
}


// called when new range is selected
void cRangeObsermatic::newRange(QVariant range)
{
    cVeinModuleParameter *pParameter = qobject_cast<cVeinModuleParameter*>(sender()); // get sender of updated signal
    int index = m_RangeParameterList.indexOf(pParameter); // which channel is it
    if (true /*!m_bRangeAutomatic*/)
    {
        QString s;
        s = range.toString();

        QList<int> chnIndexlist = getGroupIndexList(index);
        // in case of active grouping we have to set all the ranges in that group if possible
        // so we fetch a list of index for all channels in group ,in case of inactive grouping
        // the list will contain only 1 index
        // if we find 1 channel in a group that hasn't the wanted range we reset grouping !
        // let's first test if all channels have the wanted range

        for (int i = 0; i < chnIndexlist.count(); i++)
        {
            index = chnIndexlist.at(i);
            if ( m_RangeMeasChannelList.at(index)->isPossibleRange(s))
            {
                stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(index);
                sPar.m_sPar = s;
                m_ConfPar.m_senseChannelRangeParameter.replace(index, sPar);
                m_brangeSet = true;
                m_actChannelRangeNotifierList.replace(index,QString("")); // this will assure that a notification will be sent after setRanges()
            }
            else
            {
                m_ConfPar.m_nGroupAct.m_nActive = 0;
                m_bGrouping = false;
                if (m_ConfPar.m_bGrouping)
                    m_pParGroupingOnOff->setValue(0);
            }
        }

        setRanges();

        // earlier we actualized the components (entities) from here
        // it could be that we had to switch also other channels
        // or that we could not switch them because of overload conditions

        // but for sync. purpose we must set components later , after the ranges have been set
    }

    emit m_pModule->parameterChanged();
}


// called when range automatic becomes on or off
void cRangeObsermatic::newRangeAuto(QVariant rauto)
{
    bool ok;

    m_ConfPar.m_nRangeAutoAct.m_nActive = rauto.toInt(&ok);

    if ( (m_bRangeAutomatic = (rauto.toInt(&ok) == 1)) )
    {
        //qDebug() << "Range Automatic on";
        rangeAutomatic(); // call once if switched to automatic
        groupHandling(); // check for grouping
        setRanges();
    }
    else
    {
        //qDebug() << "Range Automatic off";
    }

    emit m_pModule->parameterChanged();
}


// called when grouping becomes on or off
void cRangeObsermatic::newGrouping(QVariant rgrouping)
{
    bool ok;

    m_ConfPar.m_nGroupAct.m_nActive = rgrouping.toInt(&ok);

    if ( (m_bGrouping = (rgrouping.toInt(&ok) == 1)) )
    {
        groupHandling(); // call once if switched to grouphandling
        setRanges();
    }

    emit m_pModule->parameterChanged();
}

// called when overload is reset
void cRangeObsermatic::newOverload(QVariant overload)
{
    bool ok;

    disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here

    if (overload.toInt(&ok) == 0) // allthough there is a validation for this value we only accept 0 here
    {
        if (m_ConfPar.m_bOverload) // we do something only if configured overload handling
        {
            cRangeMeasChannel *pmChn;
            for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we reset all channels
            {
                pmChn = m_RangeMeasChannelList.at(i);
                m_MsgNrCmdList[pmChn->resetStatus()] = resetstatus;
                m_hardOvlList.replace(i, false);
                m_maxOvlList.replace(i, false);
                m_pComponentOverloadMax->setValue(0);
            }

            // and then we force setting new ranges
            setRanges(true);
        }
    }

    // in each case we reset overload here
    m_pParOverloadOnOff->setValue(0);

    connect(m_pParOverloadOnOff, SIGNAL(sigValueChanged(QVariant)), SLOT(newOverload(QVariant)));
}


void cRangeObsermatic::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if (msgnr == 0) // 0 was reserved for async. messages
    {
        // that we will ignore
    }
    else
    {
        // because rangemodulemeasprogram, adjustment and rangeobsermatic share the same dsp interface
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case readgain2corr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(readdspgaincorrErrMsg)));
#ifdef DEBUG
                    qDebug() << readdspgaincorrErrMsg;
#endif
                    emit activationError();
                }
                break;
            case writegain2corr:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(writedspgaincorrErrMsg)));
#ifdef DEBUG
                    qDebug() << writedspgaincorrErrMsg;
#endif
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
    switch (cmd)
    {
    case resetstatus: // for the moment we do nothing here
        break;
    case readstatus:
        if (m_nReadStatusPending > 0)
        {
            m_nReadStatusPending--;
            if (m_nReadStatusPending == 0)
            {
                emit readStatusContinue();
            }
        }
        break;

    default:
        if ((cmd >= setrange) && (cmd <= setrange +32))
        {
            cmd -= setrange;
            // this is our synchronization..setValue emits notification
            m_RangeParameterList.at(cmd)->setValue(QVariant(m_actChannelRangeList.at(cmd)));
            m_actChannelRangeNotifierList.replace(cmd, (m_actChannelRangeList.at(cmd)));
            if (m_nRangeSetPending > 0)
            {
                m_nRangeSetPending--;
                if (m_nRangeSetPending == 0)
                {
#ifdef DEBUG
                    qDebug() << "SIG_RANGING = 0";
#endif
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

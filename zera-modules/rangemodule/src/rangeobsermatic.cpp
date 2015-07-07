#include <QString>
#include <QVector>
#include <QPoint>
#include <QJsonObject>
#include <QJsonArray>
#include <math.h>
#include <veinpeer.h>
#include <veinentity.h>
#include <dspinterface.h>
#include <proxy.h>
#include <proxyclient.h>

#include "debug.h"
#include "errormessages.h"
#include "rangemodule.h"
#include "moduleparameter.h"
#include "interfaceentity.h"
#include "moduleinfo.h"
#include "modulesignal.h"
#include "rangeobsermatic.h"
#include "rangemeaschannel.h"

namespace RANGEMODULE
{

cRangeObsermatic::cRangeObsermatic(cRangeModule *module, Zera::Proxy::cProxy* proxy, VeinPeer *peer, cSocket *dsprmsocket, QList<QStringList> groupList, QStringList chnlist, cObsermaticConfPar& confpar)
    :m_pModule(module), m_pProxy(proxy), m_pPeer(peer), m_pDSPSocket(dsprmsocket), m_GroupList(groupList), m_ChannelNameList(chnlist), m_ConfPar(confpar)
{
    m_brangeSet =false;
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
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrState);
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


void cRangeObsermatic::generateInterface()
{
    QString s;
    VeinEntity* pEntity;
    for (int i = 0; i < m_ChannelNameList.count(); i++)
    {
        pEntity = m_pPeer->dataAdd(QString("PAR_Channel%1Range").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->setValue(QVariant(s = "Unknown"));
        m_RangeEntityList.append(pEntity);

        m_actChannelRangeList.append(s); // here we also fill our internal actual channel range list
        m_ChannelAliasList.append(s); // also a list for alias names
        m_RangeMeasChannelList.append(m_pModule->getMeasChannel(m_ChannelNameList.at(i)));

        pEntity = m_pPeer->dataAdd(QString("SIG_Channel%1OVL").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->modifiersAdd(VeinEntity::MOD_READONLY);
        pEntity->setValue(QVariant((int)0), m_pPeer);
        m_RangeOVLEntityList.append(pEntity);

        pEntity = m_pPeer->dataAdd(QString("INF_Channel%1REJ").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->modifiersAdd(VeinEntity::MOD_READONLY);
        pEntity->setValue(QVariant((float)0.0), m_pPeer);
        m_RangeRejectionEntityList.append(pEntity);

        pEntity = m_pPeer->dataAdd(QString("INF_Channel%1ActREJ").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->modifiersAdd(VeinEntity::MOD_READONLY);
        pEntity->setValue(QVariant((float)0.0), m_pPeer);
        m_RangeActRejectionEntityList.append(pEntity);

        pEntity = m_pPeer->dataAdd(QString("INF_Channel%1ActOVRREJ").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->modifiersAdd(VeinEntity::MOD_READONLY);
        pEntity->setValue(QVariant((float)0.0), m_pPeer);
        m_RangeActOvrRejectionEntityList.append(pEntity);

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
          cModuleInfo *modInfo = new cModuleInfo(m_pPeer, QString("INF_Group%1").arg(i+1), QVariant(s));
          m_GroupInfoList.append(modInfo);
        }
    }

    m_pParRangeAutomaticOnOff = new cModuleParameter(m_pPeer, "PAR_RangeAutomaticON/OFF", (int)0, !m_ConfPar.m_bRangeAuto);
    m_pParGroupingOnOff = new cModuleParameter(m_pPeer, "PAR_ChannelGroupingON/OFF", (int)0, !m_ConfPar.m_bGrouping);
    m_pParOverloadOnOff = new cModuleParameter(m_pPeer, "PAR_OverloadON/OFF", (int)0, !m_ConfPar.m_bOverload);

    m_pRangingSignal = new cModuleSignal(m_pPeer, "SIG_RANGING", QVariant(0));
}


void cRangeObsermatic::deleteInterface()
{
    for (int i = 0; i < m_ChannelNameList.count(); i++) // we remove interface for all channels
    {
        m_pPeer->dataRemove(m_RangeEntityList.at(i));
        m_pPeer->dataRemove(m_RangeOVLEntityList.at(i));
        m_pPeer->dataRemove(m_RangeRejectionEntityList.at(i));
        m_pPeer->dataRemove(m_RangeActRejectionEntityList.at(i));
        m_pPeer->dataRemove(m_RangeActOvrRejectionEntityList.at(i));
    }

    for (int i = 0; i < m_GroupList.count(); i++)
        delete m_GroupInfoList.at(i);

    delete m_pParRangeAutomaticOnOff;
    delete m_pParGroupingOnOff;
    delete m_pParOverloadOnOff;

    delete m_pRangingSignal;

}


void cRangeObsermatic::exportInterface(QJsonArray &jsArr)
{
    cInterfaceEntity ifaceEntity;

    ifaceEntity.setDescription(QString("Writing this entity sets the channels range")); // for all actvalues the same
    ifaceEntity.setSCPIModel(QString("SENSE"));
    ifaceEntity.setSCPIType(QString("10"));
    ifaceEntity.setUnit(QString(""));
    ifaceEntity.setSCPICmdnode(QString("RANGE"));

    for (int i = 0; i < m_ChannelNameList.count(); i++)
    {
        ifaceEntity.setName(m_RangeEntityList.at(i)->getName());
        ifaceEntity.setAddParents(m_ChannelAliasList.at(i));

        ifaceEntity.appendInterfaceEntity(jsArr);
    }

    ifaceEntity.setAddParents("");
    ifaceEntity.setSCPIModel(QString("CONFIGURATION")); // preset

    if (m_ConfPar.m_bGrouping) // only if configured
    {
        ifaceEntity.setName(m_pParGroupingOnOff->getName());
        ifaceEntity.setDescription(QString("This entity selects channel grouping"));

        ifaceEntity.setSCPICmdnode(QString("GROUPING"));
        ifaceEntity.setUnit(QString(""));
        ifaceEntity.appendInterfaceEntity(jsArr);
    }

    if (m_ConfPar.m_bRangeAuto)
    {
        ifaceEntity.setName(m_pParRangeAutomaticOnOff->getName());
        ifaceEntity.setDescription(QString("This entity selects range automatic"));
        ifaceEntity.setSCPICmdnode(QString("RNGAUTO"));
        ifaceEntity.setUnit(QString(""));
        ifaceEntity.appendInterfaceEntity(jsArr);

    }
}


void cRangeObsermatic::rangeObservation()
{   
    bool markOverload =false;

    cRangeMeasChannel *pmChn;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we test all channels
    {
        pmChn = m_RangeMeasChannelList.at(i);

        if ( (pmChn->isOverload(m_ActualValues[i])) || m_hardOvlList.at(i)) // if any overload ?
        {
            markOverload = true;
            // we mark each overload condition if possible (range automatic) we unmark it
            // but there was an edge on this entity

                // if an overload is recovered by rangeautomatic during running measurement

            QString s = pmChn->getMaxRange();

            if (m_actChannelRangeList.at(i) == s) // in case ovrload was in max. range
                m_maxOvlList.replace(i, true);

            stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(i);
            sPar.m_sPar = s;
            m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
            // m_newChannelRangeList.replace(i, s); // set to maximum range
            m_RangeOVLEntityList.at(i)->setValue(QVariant((int)1), m_pPeer); // set interface overload
            m_softOvlList.replace(i, true); //
        }
        else
        {
            m_RangeOVLEntityList.at(i)->setValue(QVariant((int)0), m_pPeer);
            m_softOvlList.replace(i, false);
        }
    }

    disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here
    if (markOverload)
        m_pParOverloadOnOff->setData(QVariant(1));
    else
        if (m_brangeSet)
        {
            m_pParOverloadOnOff->setData(QVariant(0));
            m_brangeSet = false;
        }

    connect(m_pParOverloadOnOff, SIGNAL(updated(QVariant)), SLOT(newOverload(QVariant)));
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
                        sPar.m_sPar = pmChn->getOptRange(m_ActualValues[i]);
                        m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
                        //m_newChannelRangeList.replace(i, pmChn->getOptRange(m_ActualValues[i]));
                    }
                }

                else
                {
                    unmarkOverload = false;
                    m_MsgNrCmdList[pmChn->resetStatus()] = resetstatus;
                }
            }
        }

        if (unmarkOverload) // if we could recover all overloads
        {
            disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here
            m_pParOverloadOnOff->setData(QVariant(0));
            connect(m_pParOverloadOnOff, SIGNAL(updated(QVariant)), SLOT(newOverload(QVariant)));
        }
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
                if (m_ChannelAliasList.contains(grouplist.at(j)))
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
                    // rngUrValue = m_RangeMeasChannelList.at(indexList.at(j))->getUrValue(m_newChannelRangeList.at(indexList.at(j)));
                    int k = indexList.at(j);
                    rngUrValue = m_RangeMeasChannelList.at(k)->getUrValue(m_ConfPar.m_senseChannelRangeParameter.at(k).m_sPar);
                    if (maxUrValue < rngUrValue)
                    {
                        maxUrValue = rngUrValue;
                        maxIndex = indexList.at(j); //
                    }
                }

                // then we set all channels in grouplist to that range
                //QString newRange = m_newChannelRangeList.at(maxIndex);
                QString newRange = m_ConfPar.m_senseChannelRangeParameter.at(maxIndex).m_sPar;
                for (int j = 0; j < indexList.count(); j++)
                {
                    //m_newChannelRangeList.replace(indexList.at(j), newRange);
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
        //s = m_newChannelRangeList.at(i);
        s = m_ConfPar.m_senseChannelRangeParameter.at(i).m_sPar;
        pmChn = m_RangeMeasChannelList.at(i);
        if (! pmChn->isPossibleRange(s)) // we test whether this range is possible, otherwise we take the max. range
        {
            s = pmChn->getMaxRange();
            //m_newChannelRangeList.replace(i, s);

            stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(i);
            sPar.m_sPar = s;
            m_ConfPar.m_senseChannelRangeParameter.replace(i, sPar);
        }

        if ( s != m_actChannelRangeList.at(i) || force)
        {
            if (!change) // signal is only set once unless there are more than 1 ranges to change
            {
#ifdef DEBUG
                qDebug() << "SIG_RANGING = 1";
#endif
                m_pRangingSignal->m_pParEntity->setValue(QVariant(1), m_pPeer);
            }

            change = true;

            // if we have an overload condition in channel we reset it before we set the new range
            if (m_hardOvlList.at(i) && ! m_maxOvlList.at(i))
            {
                m_MsgNrCmdList[pmChn->resetStatus()] = resetstatus;
                m_hardOvlList.replace(i, false);
            }

            m_MsgNrCmdList[pmChn->setRange(s)] = setrange;
            m_nRangeSetPending++;

            m_actChannelRangeList.replace(i, s);
            VeinEntity* pEntity = m_RangeEntityList.at(i);
            disconnect(m_RangeEntityList.at(i), 0, this, 0); // we don't want a signal when we switch the range by ourself
            pEntity->setValue(QVariant(s),m_pPeer);
            connect(m_RangeEntityList.at(i), SIGNAL(sigValueChanged(QVariant)), SLOT(newRange(QVariant)));
            chn = pmChn->getDSPChannelNr();
            m_pfScale[chn] = pmChn->getUrValue() / pmChn->getRejection();

            m_RangeActRejectionEntityList.at(i)->setValue(pmChn->getUrValue(), m_pPeer); // we first set information of channels actual urvalue
            m_RangeActOvrRejectionEntityList.at(i)->setValue(pmChn->getRangeUrvalueMax(), m_pPeer); // we additional set information of channels actual urvalue incl. reserve

#ifdef DEBUG
            qDebug() << QString("setRange Ch%1; %2; Scale=%3").arg(chn).arg(s).arg(m_pfScale[chn]);
#endif
        }
    }

    if (change && !m_writeCorrectionDSPMachine.isRunning())

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

    // after activation we reset the pending counter
    // because signal/slot connections are not yet established
    // and so we don't get signals from ranges that were set
    m_nRangeSetPending = 0;

    // we read all gain2corrections, set default ranges, default automatic, grouping and scaling values
    // lets now connect signals so we become alive
    for (int i = 0; i < m_ChannelNameList.count(); i++)
        connect(m_RangeEntityList.at(i), SIGNAL(sigValueChanged(QVariant)), SLOT(newRange(QVariant)));

    connect(m_pParRangeAutomaticOnOff, SIGNAL(updated(QVariant)), this, SLOT(newRangeAuto(QVariant)));
    connect(m_pParGroupingOnOff, SIGNAL(updated(QVariant)), SLOT(newGrouping(QVariant)));
    connect(m_pParOverloadOnOff, SIGNAL(updated(QVariant)), SLOT(newOverload(QVariant)));

    cRangeMeasChannel *pmChn;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++)
    {
        pmChn = m_RangeMeasChannelList.at(i);
        m_RangeRejectionEntityList.at(i)->setValue(pmChn->getMaxRangeUrvalueMax(), m_pPeer);
    }

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
    VeinEntity *entity = qobject_cast<VeinEntity*>(sender()); // get sender of updated signal
    int index = m_RangeEntityList.indexOf(entity); // which channel is it
    if (true /*!m_bRangeAutomatic*/)
    {
        QString s;
        float attenuation;
        s = range.toString();

        QList<int> chnIndexlist = getGroupIndexList(index);
        // in case of active grouping we have to set all the ranges in that group if possible
        // so we fetch a list for all channels in group of index

        for (int i = 0; i < chnIndexlist.count(); i++)
        {
            index = chnIndexlist.at(i);
            if ( (m_ActualValues.size() > 0) && (m_ActualValues.size() >= index) )
                attenuation = m_ActualValues[index];
            else
                attenuation = 0.0; // if we don't have any actual values yet

            // lets first test that new range will not cause an overload condition in this channel
            if (m_RangeMeasChannelList.at(index)->isPossibleRange(s , attenuation))
            {
                //m_newChannelRangeList.replace(index, s);
                stringParameter sPar = m_ConfPar.m_senseChannelRangeParameter.at(index);
                sPar.m_sPar = s;
                m_ConfPar.m_senseChannelRangeParameter.replace(index, sPar);
                m_brangeSet = true;
            }
        }

        groupHandling();
        setRanges();

        // we actualize the entities because it maybe that we had to switch also other channels
        // or that we could not switch them because of overload conditions
        for (int i = 0; i < chnIndexlist.count(); i++)
        {
            index = chnIndexlist.at(i);
            disconnect(m_RangeEntityList.at(index), 0, this, 0); // but we don't want a signal when we switch the range entity by ourself
            entity->setValue(QVariant(m_actChannelRangeList.at(index)), m_pPeer);
            connect(m_RangeEntityList.at(index), SIGNAL(sigValueChanged(QVariant)), SLOT(newRange(QVariant)));
        }
    }
}


// called when range automatic becomes on or off
void cRangeObsermatic::newRangeAuto(QVariant rauto)
{
    bool ok;

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
}


// called when grouping becomes on or off
void cRangeObsermatic::newGrouping(QVariant rgrouping)
{
    bool ok;

    if ( (m_bGrouping = (rgrouping.toInt(&ok) == 1)) )
    {
        groupHandling(); // call once if switched to grouphandling
        setRanges();
    }
}

// called when overload is reset
void cRangeObsermatic::newOverload(QVariant overload)
{
    bool ok;

    if (overload.toInt(&ok) == 0)
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
            }

            // and then we force setting new ranges
            setRanges(true);
        }
    }
    else
    {   // the user can't set overload ... so we reset it
        disconnect(m_pParOverloadOnOff, 0, this, 0); // we don't want a signal here
        m_pParOverloadOnOff->setData(0);
        connect(m_pParOverloadOnOff, SIGNAL(updated(QVariant)), SLOT(newOverload(QVariant)));
    }
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
    case setrange:
        if (m_nRangeSetPending > 0)
        {
            m_nRangeSetPending--;
            if (m_nRangeSetPending == 0)
            {
#ifdef DEBUG
                qDebug() << "SIG_RANGING = 0";
#endif
                m_pRangingSignal->m_pParEntity->setValue(QVariant(0), m_pPeer);
            }
        }
        break;
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
    }
}

}

#include <QString>
#include <QVector>
#include <QPoint>
#include <math.h>
#include <veinpeer.h>
#include <veinentity.h>
#include <dspinterface.h>

#include "rangemodule.h"
#include "moduleparameter.h"
#include "moduleinfo.h"
#include "modulesignal.h"
#include "rangeobsermatic.h"
#include "rangemeaschannel.h"


cRangeObsermatic::cRangeObsermatic(cRangeModule *module, VeinPeer *peer, Zera::Server::cDSPInterface* iface, QList<QStringList> groupList, QStringList chnlist, QStringList rnglist, quint8 rangeauto, quint8 grouping)
    :m_pModule(module), m_pPeer(peer), m_pDSPIFace(iface), m_GroupList(groupList), m_ChannelNameList(chnlist), m_newChannelRangeList(rnglist), m_nDefaultRangeAuto(rangeauto), m_nDefaultGrouping(grouping)
{
    // temporary
    m_nRangeSetPending = 0;

    generateInterface();

    m_readGainCorrState.addTransition(this, SIGNAL(activationContinue()), &m_readGainCorrDoneState);
    m_readCorrectionDSPMachine.addState(&m_readGainCorrState);
    m_readCorrectionDSPMachine.addState(&m_readGainCorrDoneState);
    m_readCorrectionDSPMachine.setInitialState(&m_readGainCorrState);
    connect(&m_readGainCorrState, SIGNAL(entered()), SLOT(readGainCorr()));
    connect(&m_readGainCorrDoneState, SIGNAL(entered()), SLOT(readGainCorrDone()));

    m_writeGainCorrState.addTransition(this, SIGNAL(activationContinue()), &m_writeGainCorrDoneState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrState);
    m_writeCorrectionDSPMachine.addState(&m_writeGainCorrDoneState);
    m_writeCorrectionDSPMachine.setInitialState(&m_writeGainCorrState);
    connect(&m_writeGainCorrState, SIGNAL(entered()), SLOT(writeGainCorr()));
    connect(&m_writeGainCorrDoneState, SIGNAL(entered()), SLOT(writeGainCorrDone()));
}


cRangeObsermatic::~cRangeObsermatic()
{
    deactivate();
    deleteInterface();
}


void cRangeObsermatic::activate()
{
    // the alias list is correctly filled when activating range obsermatic
    // the module has first activated the channels before activating rangeobsermatic
    for (int i = 0; i < m_ChannelNameList.count(); i++)
        m_ChannelAliasList.replace(i, m_RangeMeasChannelList.at(i)->getAlias());

    m_pGainCorrection2DSP = m_pDSPIFace->getMemHandle("SCALEMEM");
    m_pGainCorrection2DSP->addVarItem( new cDspVar("GAINCORRECTION2",32, DSPDATA::vDspIntVar));
    m_pfScale =  m_pDSPIFace->data(m_pGainCorrection2DSP, "GAINCORRECTION2");

    connect(m_pDSPIFace, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_readCorrectionDSPMachine.start(); // we read all correction data once
}


void cRangeObsermatic::deactivate()
{
    disconnect(m_pDSPIFace, 0, this, 0); // we disconnect from our dsp interface
    m_pDSPIFace->deleteMemHandle(m_pGainCorrection2DSP); // and free our memory handle
    emit deactivated();
}


void cRangeObsermatic::ActionHandler(QVector<float> *actualValues)
{
    m_ActualValues = *actualValues;
    // qDebug() << "range obsermatic new actual values";
    // qDebug() << QString("PEAK %1 ; %2 ; %3").arg(m_ActualValues[0]).arg(m_ActualValues[1]).arg(m_ActualValues[2]);
    // qDebug() << QString("RMS %1 ; %2 ; %3").arg(m_ActualValues[6]).arg(m_ActualValues[7]).arg(m_ActualValues[8]);

    rangeObservation(); // first we test for overload conditions
    rangeAutomatic(); // let rangeautomatic do its job
    groupHandling(); // and look for grouping channels if necessary
    setRanges(); // set the new ranges now
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
        // m_newChannelRangeList is set within constructor
        m_ChannelAliasList.append(s); // also a list for alias names
        m_RangeMeasChannelList.append(m_pModule->getMeasChannel(m_ChannelNameList.at(i)));

        pEntity = m_pPeer->dataAdd(QString("SIG_Channel%1OVL").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->modifiersAdd(VeinEntity::MOD_READONLY);
        pEntity->setValue(QVariant((int)0), m_pPeer);
        m_RangeOVLEntityList.append(pEntity);

        m_OvlList.append(false);
    }

    for (int i = 0; i < m_GroupList.count(); i++)
    {
      cModuleInfo *modInfo = new cModuleInfo(m_pPeer, QString("INF_Group%1").arg(i+1), QVariant(m_GroupList.at(i)));
      m_GroupInfoList.append(modInfo);
    }

    m_pParRangeAutomaticOnOff = new cModuleParameter(m_pPeer, "PAR_RangeAutomaticON/OFF", (int)-1, "RangeAutomaticLimits", QVariant(QPoint(1,0)));
    m_pParGroupingOnOff = new cModuleParameter(m_pPeer, "PAR_ChannelGroupingON/OFF", (int)-1, "ChannelGroupingLimits", QVariant(QPoint(1,0)));
    m_pRangingSignal = new cModuleSignal(m_pPeer, "SIG_RANGING", QVariant(0));
}


void cRangeObsermatic::deleteInterface()
{
    for (int i = 0; i < m_ChannelNameList.count(); i++) // we remove interface for all channels
    {
        m_pPeer->dataRemove(m_RangeEntityList.at(i));
        m_pPeer->dataRemove(m_RangeOVLEntityList.at(i));
    }

    for (int i = 0; i < m_GroupList.count(); i++)
        delete m_GroupInfoList.at(i);


    // we need not clear our lists if deleteInterface is called from destructor
    /*
    m_RangeEntityList.clear();
    m_RangeOVREntityList.clear();
    m_actChannelRangeList.clear();
    m_newChannelRangeList.clear();
    m_RangeMeasChannelList.clear();
    m_ChannelAliasList.clear();
    m_OvlList.clear();
    m_GroupInfoList.clear();
    */

    delete m_pParRangeAutomaticOnOff;
    delete m_pParGroupingOnOff;

}


void cRangeObsermatic::rangeObservation()
{
    cRangeMeasChannel *pmChn;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we test all channels
    {
        pmChn = m_RangeMeasChannelList.at(i);

        if (pmChn->isOverload(m_ActualValues[i])) // if overload ?
        {
            QString s = m_newChannelRangeList.at(i);
            m_newChannelRangeList.replace(i, pmChn->getMaxRange()); // set to maximum range
            // qDebug() << "Overload " << pmChn->getAlias() << QString("Act: %1").arg(m_ActualValues[i]) << QString("OldRng: %1").arg(s) << QString("NewRng: %1").arg(m_newChannelRangeList.at(i));

            m_RangeOVLEntityList.at(i)->setValue(QVariant((int)1), m_pPeer); // set interface overload
            m_OvlList.replace(i, true); //
        }
        else
        {
            m_RangeOVLEntityList.at(i)->setValue(QVariant((int)0), m_pPeer);
            m_OvlList.replace(i, false);
        }
    }
}


void cRangeObsermatic::rangeAutomatic()
{
    if (m_bRangeAutomatic)
    {
        cRangeMeasChannel *pmChn;
        for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we test all channels
        {
            if (!m_OvlList.at(i)) // only range automatic if there is no overload
            {
                pmChn = m_RangeMeasChannelList.at(i);
                m_newChannelRangeList.replace(i, pmChn->getOptRange(m_ActualValues[i])); // set to optimum range
            }
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
                int maxIndex;

                // first we search for the range with max upper range value
                for (int j = 0; j < indexList.count(); j++)
                {
                    rngUrValue = m_RangeMeasChannelList.at(indexList.at(j))->getUrValue(m_newChannelRangeList.at(indexList.at(j)));
                    if (maxUrValue < rngUrValue)
                    {
                        maxUrValue = rngUrValue;
                        maxIndex = indexList.at(j); //
                    }
                }

                // then we set all channels in grouplist to that range
                QString newRange = m_newChannelRangeList.at(maxIndex);
                for (int j = 0; j < indexList.count(); j++)
                    m_newChannelRangeList.replace(indexList.at(j), newRange);

            }
        }
    }
}


void cRangeObsermatic::setRanges()
{
    cRangeMeasChannel *pmChn;
    QString s;
    quint8 chn;
    bool change;

    change = false;
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we set all channels if needed
    {
        if ( (s = m_newChannelRangeList.at(i)) != m_actChannelRangeList.at(i))
        {
            change = true;
            pmChn = m_RangeMeasChannelList.at(i);

            m_pRangingSignal->m_pParEntity->setValue(QVariant(1), m_pPeer);
            m_MsgNrCmdList[pmChn->setRange(s)] = RANGEOBSERMATIC::setrange;
            m_nRangeSetPending++;

            m_actChannelRangeList.replace(i, s);
            VeinEntity* pEntity = m_RangeEntityList.at(i);
            disconnect(m_RangeEntityList.at(i), 0, this, 0); // we don't want a signal when we switch the range by ourself
            pEntity->setValue(QVariant(s));
            connect(m_RangeEntityList.at(i), SIGNAL(sigValueChanged(QVariant)), SLOT(newRange(QVariant)));
            chn = pmChn->getDSPChannelNr();
            m_pfScale[chn] = pmChn->getUrValue() / pmChn->getRejection();
            //if (chn == 0)
            //    qDebug() << QString("setRange Ch%1; %2; Scale=%3").arg(chn).arg(s).arg(m_pfScale[chn]);
        }
    }

    if (change && !m_writeCorrectionDSPMachine.isRunning())

        m_writeCorrectionDSPMachine.start(); // we write all correction after each range setting
}


void cRangeObsermatic::readGainCorr()
{
    // qDebug() << "readGainCorr";
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryRead(m_pGainCorrection2DSP)] = RANGEOBSERMATIC::readgain2corr;
}


void cRangeObsermatic::readGainCorrDone()
{
    setRanges();
    m_bRangeAutomatic = (m_nDefaultRangeAuto == 1);
    m_bGrouping = (m_nDefaultGrouping == 1);
    // we read all gain2corrections, set default ranges, default automatic, grouping and scaling values
    // lets now connect signals so we become alive
    for (int i = 0; i < m_ChannelNameList.count(); i++)
        connect(m_RangeEntityList.at(i), SIGNAL(sigValueChanged(QVariant)), SLOT(newRange(QVariant)));

    connect(m_pParRangeAutomaticOnOff, SIGNAL(updated(QVariant)), this, SLOT(newRangeAuto(QVariant)));
    connect(m_pParGroupingOnOff, SIGNAL(updated(QVariant)), SLOT(newGrouping(QVariant)));

    emit activated();
}


void cRangeObsermatic::writeGainCorr()
{
    // qDebug() << "writeGainCorr";
    m_MsgNrCmdList[m_pDSPIFace->dspMemoryWrite(m_pGainCorrection2DSP)] = RANGEOBSERMATIC::writegain2corr;
}


void cRangeObsermatic::writeGainCorrDone()
{
    // qDebug() << "writeGainCorrDone";
}


void cRangeObsermatic::newRange(QVariant range)
{
    VeinEntity *entity = qobject_cast<VeinEntity*>(sender()); // get sender of updated signal
    int index = m_RangeEntityList.indexOf(entity); // which channel is it
    if (true /*!m_bRangeAutomatic*/)
    {
        QString s;
        float attenuation;
        s = range.toString();


        if ( (m_ActualValues.size() > 0) && (m_ActualValues.size() >= index) )
            attenuation = m_ActualValues[index];
        else
            attenuation = 0.0; // if we don't have any actual values yet

        // lets first test that new range will cause no overload condition
        if (m_RangeMeasChannelList.at(index)->isPossibleRange(s , attenuation))
        {
            m_newChannelRangeList.replace(index, s);
            groupHandling();
            setRanges();
            return;
        }
        else
        {
            disconnect(m_RangeEntityList.at(index), 0, this, 0); // we don't want a signal when we switch the range by ourself
            entity->setValue(range, m_pPeer);
            entity->setValue(QVariant(m_actChannelRangeList.at(index)), m_pPeer);
            connect(m_RangeEntityList.at(index), SIGNAL(sigValueChanged(QVariant)), SLOT(newRange(QVariant)));
        }
    }

    // in any case if we could not set the new range we set interface entity to actual range
    entity->setValue(m_actChannelRangeList.at(index));
}


void cRangeObsermatic::newRangeAuto(QVariant rauto)
{
    bool ok;

    if ((m_bRangeAutomatic = rauto.toInt(&ok) == 1))
    {
        rangeAutomatic(); // call once if switched to automatic
        groupHandling(); // check for grouping
        setRanges();
    }
}


void cRangeObsermatic::newGrouping(QVariant rgrouping)
{
    bool ok;

    if ((m_bGrouping = rgrouping.toInt(&ok) == 1))
    {
        groupHandling(); // call once if switched to grouphandling
        setRanges();
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
            case RANGEOBSERMATIC::readgain2corr:
            case RANGEOBSERMATIC::writegain2corr:
                if (reply == ack)
                    emit activationContinue();
                else
                    emit activationError();
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
    case RANGEOBSERMATIC::setrange:
        if (m_nRangeSetPending > 0)
        {
            m_nRangeSetPending--;
            if (m_nRangeSetPending == 0)
                m_pRangingSignal->m_pParEntity->setValue(QVariant(0), m_pPeer);
        }
        break;
    }
}



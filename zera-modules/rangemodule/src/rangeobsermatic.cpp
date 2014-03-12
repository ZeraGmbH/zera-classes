#include <QString>
#include <QVector>
#include <QPoint>
#include <math.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "rangemodule.h"
#include "moduleparameter.h"
#include "moduleinfo.h"
#include "rangeobsermatic.h"
#include "rangemeaschannel.h"


cRangeObsermatic::cRangeObsermatic(cRangeModule *module, VeinPeer *peer, QList<QStringList> groupList, QStringList chnlist)
    :m_pModule(module), m_pPeer(peer), m_GroupList(groupList), m_ChannelNameList(chnlist)
{
    generateInterface();
}


cRangeObsermatic::~cRangeObsermatic()
{
    deleteInterface();
}


void cRangeObsermatic::activate()
{
    // the alias list is correctly filled when activating range obsermatic
    // the module has first activated the channels before activating rangeobsermatic
    for (int i = 0; i < m_ChannelNameList.count(); i++)
        m_ChannelAliasList.replace(i, m_RangeMeasChannelList.at(i)->getAlias());

    for (int i = 0; i < m_ChannelNameList.count(); i++)
        connect(m_RangeEntityList.at(i), SIGNAL(updated()), SLOT(newRange()));

    connect(m_pParRangeAutomaticOnOff, SIGNAL(updated()), this, SLOT(newRangeAuto()));
    connect(m_pParGroupingOnOff, SIGNAL(updated()), SLOT(newGrouping()));
}


void cRangeObsermatic::deactivate()
{
}


void cRangeObsermatic::ActionHandler(QVector<double> *actualValues)
{
    m_ActualValues = *actualValues;

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
        pEntity = m_pPeer->dataAdd(QString("Channel%1Range").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->setValue(QVariant(s = "Unknown"));
        m_RangeEntityList.append(pEntity);
        m_actChannelRangeList.append(s); // here we also fill our internal actual channel range list
        m_newChannelRangeList.append(s); // also the list for new ranges
        m_ChannelAliasList.append(s); // also a list for alias names
        m_RangeMeasChannelList.append(m_pModule->getMeasChannel(m_ChannelNameList.at(i)));
        pEntity = m_pPeer->dataAdd(QString("Channel%1OVL").arg(i+1));
        pEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
        pEntity->modifiersAdd(VeinEntity::MOD_READONLY);
        pEntity->setValue(QVariant((int)0));
        m_RangeOVLEntityList.append(pEntity);
        m_OvlList.append(false);
    }

    for (int i = 0; i < m_GroupList.count(); i++)
    {
      cModuleInfo *modInfo = new cModuleInfo(m_pPeer, QString("Group%1").arg(i+1), QVariant(m_GroupList.at(i)));
      m_GroupInfoList.append(modInfo);
    }

    m_pParRangeAutomaticOnOff = new cModuleParameter(m_pPeer, "RangeAutomatic ON/OFF", (int)-1, "RangeAutomaticLimits", QVariant(QPoint(1,0)));
    m_pParGroupingOnOff = new cModuleParameter(m_pPeer, "ChannelGrouping ON/OFF", (int)-1, "ChannelGroupingLimits", QVariant(QPoint(1,0)));
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
            m_newChannelRangeList.replace(i, pmChn->getMaxRange()); // set to maximum range
            m_RangeOVLEntityList.at(i)->setValue(QVariant((int)1)); // set interface overload
            m_OvlList.replace(i, true); //
        }
        else
        {
            m_RangeOVLEntityList.at(i)->setValue(QVariant((int)0));
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
                        maxIndex = j; //
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
    for (int i = 0; i < m_RangeMeasChannelList.count(); i++) // we set all channels if needed
    {
        if (m_newChannelRangeList.at(i) != (s = m_actChannelRangeList.at(i)))
        {
            pmChn = m_RangeMeasChannelList.at(i);
            pmChn->setRange(s);
            m_actChannelRangeList.replace(i, s);
            m_RangeEntityList.at(i)->setValue(QVariant(s));
        }
    }
}


void cRangeObsermatic::newRange()
{
    VeinEntity *entity = qobject_cast<VeinEntity*>(sender()); // get sender of updated signal
    int index = m_RangeEntityList.indexOf(entity); // which channel is it
    if (!m_bRangeAutomatic)
    {
        QString s;
        s = entity->getValue().toString();
        // lets first test that new range will cause no overload condition
        if (m_RangeMeasChannelList.at(index)->isPossibleRange(s , m_ActualValues[index]))
        {
            m_newChannelRangeList.replace(index, s);
            groupHandling();
            setRanges();
            return;
        }
    }

    // in any case if we could not set the new range we set interface entity to actual range
    entity->setValue(m_actChannelRangeList.at(index));
}


void cRangeObsermatic::newRangeAuto()
{
    bool ok;

    if ((m_bRangeAutomatic = m_pParRangeAutomaticOnOff->getData().toInt(&ok) == 1))
    {
        rangeAutomatic(); // call once if switched to automatic
        groupHandling(); // check for grouping
        setRanges();
    }
}


void cRangeObsermatic::newGrouping()
{
    bool ok;

    if ((m_bGrouping = m_pParGroupingOnOff->getData().toInt(&ok) == 1))
    {
        groupHandling(); // call once if switched to grouphandling
        setRanges();
    }
}





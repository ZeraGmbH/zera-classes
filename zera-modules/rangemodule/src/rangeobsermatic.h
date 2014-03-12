#ifndef RANGEOBSERMATIC_H
#define RANGEOBSERMATIC_H


// cRangeObsermatic is responsible for setting, observing, automatic setting
// and grouping measurement channel ranges. it works on a flexible list of
// system measurement channels.
// it provides its interface :
// ChannelXRange Input/Output for range setting
// ChannelXOVR   Output information of overload conditions
// RangeAutomatic ON/OFF Input
// GroupHandling ON/OFF Input
// !!! important range obsermatic must be activated after !!! activating rangemeaschannels

#include <QObject>
#include <QList>
#include <QVector>
#include <QStringList>

#include "moduleacitvist.h"

class VeinPeer;
class VeinEntity;
class cRangeModule;
class cModuleParameter;
class cModuleInfo;
class cRangeMeasChannel;

class cRangeObsermatic: public cModuleActivist
{
    Q_OBJECT

public:
    cRangeObsermatic(cRangeModule* module, VeinPeer* peer, QList<QStringList> groupList,QStringList chnlist);
    virtual ~cRangeObsermatic();

public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right
    virtual void ActionHandler(QVector<double>* actualValues); // entry after received actual values

protected:
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

private:
    cRangeModule *m_pModule;
    VeinPeer* m_pPeer;
    QList<QStringList> m_GroupList;
    QStringList m_ChannelNameList; // the system names of our channels
    QStringList m_ChannelAliasList; // the alias of our channels
    QList<cRangeMeasChannel*> m_RangeMeasChannelList;
    QList<bool> m_OvlList;
    QStringList m_actChannelRangeList; // a list of the actual ranges set
    QStringList m_newChannelRangeList; // a list of new ranges we want to get set
    QVector<double> m_ActualValues; // here we find the actual values

    // our interface entities
    QList<VeinEntity*> m_RangeEntityList;
    QList<VeinEntity*> m_RangeOVLEntityList;
    QList<cModuleInfo*> m_GroupInfoList;
    cModuleParameter* m_pParRangeAutomaticOnOff;
    cModuleParameter* m_pParGroupingOnOff;

    bool m_bRangeAutomatic;
    bool m_bGrouping;
    void rangeObservation();
    void rangeAutomatic();
    void groupHandling();
    void setRanges(); // here we really set ranges

private slots:
    void newRange();
    void newRangeAuto();
    void newGrouping();
};

#endif // RANGEOBSERMATIC_H

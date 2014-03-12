#ifndef RANGEMODULEINTERFACEDATA_H
#define RANGEMODULEINTERFACEDATA_H

class cRangeModuleInterfaceData
{
public:
    cRangeModuleInterfaceData(){}
    // interface parameter
    VeinEntity *m_pGroupAct;
    VeinEntity *m_pGroupActLimits; // 0,1
    VeinEntity *m_pAutoAct;
    VeinEntity *m_pAutoActLimits; // 0,1
    VeinEntity *m_pInterval;
    VeinEntity *m_pIntervalLimits; // 0.1 <= interval <= 2.0

    // interface measuring channels
    QHash<QString, VeinEntity*> m_channelEntityList; // e.g. ...["m0"] = UL1
    QHash<QString, VeinEntity*> m_channelrangeEntityList;
    QHash<QString, VeinEntity*> m_channelrangelistEntityList;

    // interface measuring actual values
    QHash<QString, VeinEntity*> m_channelActualValueList;
};


#endif // RANGEMODULEINTERFACEDATA_H

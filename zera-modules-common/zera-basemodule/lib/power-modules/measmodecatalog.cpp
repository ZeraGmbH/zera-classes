#include "measmodecatalog.h"

QHash<QString,cMeasModeInfo> MeasModeCatalog::m_modeInfoHash;

const cMeasModeInfo &MeasModeCatalog::getInfo(QString name)
{
    if(m_modeInfoHash.isEmpty()) {
        m_modeInfoHash["4LW"] = cMeasModeInfo("4LW", "P", "W", actPower, m4lw);
        m_modeInfoHash["4LB"] = cMeasModeInfo("4LB", "Q", "Var", reactPower, m4lb);
        m_modeInfoHash["4LBK"] = cMeasModeInfo("4LBK", "Q", "Var", reactPower, m4lbk);
        m_modeInfoHash["4LS"] = cMeasModeInfo("4LS", "S", "VA", appPower, m4ls);
        m_modeInfoHash["4LSg"] = cMeasModeInfo("4LSg", "S", "W", appPower, m4lsg);
        m_modeInfoHash["3LW"] = cMeasModeInfo("3LW", "P", "W", actPower, m3lw);
        m_modeInfoHash["3LB"] = cMeasModeInfo("3LB", "Q", "Var", reactPower, m3lb);
        m_modeInfoHash["2LW"] = cMeasModeInfo("2LW", "P", "W", actPower, m2lw);
        m_modeInfoHash["2LB"] = cMeasModeInfo("2LB", "Q", "Var", reactPower, m2lb);
        m_modeInfoHash["2LS"] = cMeasModeInfo("2LS", "S", "VA", appPower, m2ls);
        m_modeInfoHash["2LSg"] = cMeasModeInfo("2LSg", "S", "VA", appPower, m2lsg);
        m_modeInfoHash["XLW"] = cMeasModeInfo("XLW", "P", "W", actPower, mXlw);
        m_modeInfoHash["QREF"] = cMeasModeInfo("QREF", "P", "W", actPower, mqref);
    }
    if(!m_modeInfoHash.contains(name)) {
        qWarning("Unknown measurement mode %s - returning 4LW", qPrintable(name));
        return m_modeInfoHash["4LW"];
    }
    return m_modeInfoHash[name];
}

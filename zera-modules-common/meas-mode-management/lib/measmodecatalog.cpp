#include "measmodecatalog.h"

QHash<QString,cMeasModeInfo> MeasModeCatalog::m_modeInfoHash;
std::function<void(QString mode)> MeasModeCatalog::m_warningHandler = [](QString name) {
    qWarning("Unknown measurement mode %s", qPrintable(name));
};

cMeasModeInfo MeasModeCatalog::getInfo(QString name)
{
    if(m_modeInfoHash.isEmpty()) {
        m_modeInfoHash["4LW"] = cMeasModeInfo("4LW", "P", "W", m4lw);
        m_modeInfoHash["4LB"] = cMeasModeInfo("4LB", "Q", "Var", m4lb);
        m_modeInfoHash["4LBK"] = cMeasModeInfo("4LBK", "Q", "Var", m4lbk);
        m_modeInfoHash["4LS"] = cMeasModeInfo("4LS", "S", "VA", m4ls);
        m_modeInfoHash["4LSg"] = cMeasModeInfo("4LSg", "S", "W", m4lsg);
        m_modeInfoHash["3LW"] = cMeasModeInfo("3LW", "P", "W", m3lw);
        m_modeInfoHash["3LB"] = cMeasModeInfo("3LB", "Q", "Var", m3lb);
        m_modeInfoHash["2LW"] = cMeasModeInfo("2LW", "P", "W", m2lw);
        m_modeInfoHash["2LB"] = cMeasModeInfo("2LB", "Q", "Var", m2lb);
        m_modeInfoHash["2LS"] = cMeasModeInfo("2LS", "S", "VA", m2ls);
        m_modeInfoHash["2LSg"] = cMeasModeInfo("2LSg", "S", "VA", m2lsg);
        m_modeInfoHash["XLW"] = cMeasModeInfo("XLW", "P", "W", mXlw);
        m_modeInfoHash["XLB"] = cMeasModeInfo("XLB", "Q", "Var", mXlb);
        m_modeInfoHash["XLS"] = cMeasModeInfo("XLS", "S", "VA", mXls);
        m_modeInfoHash["QREF"] = cMeasModeInfo("QREF", "P", "W", mqref);
    }
    if(!m_modeInfoHash.contains(name)) {
        m_warningHandler(name);
        return cMeasModeInfo();
    }
    return m_modeInfoHash[name];
}

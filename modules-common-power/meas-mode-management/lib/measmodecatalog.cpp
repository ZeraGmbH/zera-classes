#include "measmodecatalog.h"

QHash<QString,cMeasModeInfo> MeasModeCatalog::m_modeInfoHashByString;
QHash<measmodes,cMeasModeInfo> MeasModeCatalog::m_modeInfoHashById;
QSet<measmodes> MeasModeCatalog::m_threeWireSet;

std::function<void(QString mode)> MeasModeCatalog::m_warningHandler = [](QString name) {
    qWarning("Unknown measurement mode %s", qPrintable(name));
};

void MeasModeCatalog::setupHashes()
{
    addInfoToHashes(cMeasModeInfo("4LW", "P", "W", m4lw));
    addInfoToHashes(cMeasModeInfo("4LB", "Q", "Var", m4lb));
    addInfoToHashes(cMeasModeInfo("4LBK", "Q", "Var", m4lbk));
    addInfoToHashes(cMeasModeInfo("4LS", "S", "VA", m4ls));
    addInfoToHashes(cMeasModeInfo("4LSg", "S", "W", m4lsg));
    addInfoToHashes(cMeasModeInfo("3LW", "P", "W", m3lw));
    addInfoToHashes(cMeasModeInfo("3LB", "Q", "Var", m3lb));
    addInfoToHashes(cMeasModeInfo("3LS", "S", "VA", m3ls));
    addInfoToHashes(cMeasModeInfo("2LW", "P", "W", m2lw));
    addInfoToHashes(cMeasModeInfo("2LB", "Q", "Var", m2lb));
    addInfoToHashes(cMeasModeInfo("2LS", "S", "VA", m2ls));
    addInfoToHashes(cMeasModeInfo("2LSg", "S", "VA", m2lsg));
    addInfoToHashes(cMeasModeInfo("XLW", "P", "W", mXlw));
    addInfoToHashes(cMeasModeInfo("XLB", "Q", "Var", mXlb));
    addInfoToHashes(cMeasModeInfo("XLS", "S", "VA", mXls));
    addInfoToHashes(cMeasModeInfo("XLSg", "S", "VA", mXlsg));
    addInfoToHashes(cMeasModeInfo("QREF", "", "", mqref));
}

void MeasModeCatalog::addInfoToHashes(cMeasModeInfo info)
{
    m_modeInfoHashByString[info.getName()] = info;
    m_modeInfoHashById[info.getCode()] = info;
}

cMeasModeInfo MeasModeCatalog::getInfo(QString name)
{
    if(m_modeInfoHashByString.isEmpty())
        setupHashes();
    if(!m_modeInfoHashByString.contains(name)) {
        m_warningHandler(name);
        return cMeasModeInfo();
    }
    return m_modeInfoHashByString[name];
}

cMeasModeInfo MeasModeCatalog::getInfo(measmodes modeId)
{
    if(m_modeInfoHashById.isEmpty())
        setupHashes();
    if(!m_modeInfoHashById.contains(modeId))
        return cMeasModeInfo(); // no warning - we test this
    return m_modeInfoHashById[modeId];
}

QSet<measmodes> MeasModeCatalog::getThreeWireModes()
{
    if(m_threeWireSet.isEmpty()) {
        m_threeWireSet.insert(m3lw);
        m_threeWireSet.insert(m3lb);
        m_threeWireSet.insert(m3ls);
    }
    return m_threeWireSet;
}

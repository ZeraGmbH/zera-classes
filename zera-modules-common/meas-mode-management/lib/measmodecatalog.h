#ifndef MEASMODECATALOG_H
#define MEASMODECATALOG_H

#include "measmodeinfo.h"
#include <QHash>
#include <functional>

class MeasModeCatalog
{
public:
    static cMeasModeInfo getInfo(QString name);
    static cMeasModeInfo getInfo(measmodes modeId);
protected:
    static std::function<void(QString mode)> m_warningHandler;
private:
    static QHash<QString,cMeasModeInfo> m_modeInfoHashByString;
    static QHash<measmodes,cMeasModeInfo> m_modeInfoHashById;
    static void setupHashes();
    static void addInfoToHashes(cMeasModeInfo info);
};

#endif // MEASMODECATALOG_H

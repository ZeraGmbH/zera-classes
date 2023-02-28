#ifndef MEASMODECATALOG_H
#define MEASMODECATALOG_H

#include "measmodeinfo.h"
#include <QHash>
#include <functional>

class MeasModeCatalog
{
public:
    static const cMeasModeInfo *getInfoSafe(QString name);
    static const cMeasModeInfo *getInfo(QString name);
protected:
    static std::function<const cMeasModeInfo *(QString mode)> m_warningHandler;
private:
    static QHash<QString,cMeasModeInfo> m_modeInfoHash;
};

#endif // MEASMODECATALOG_H

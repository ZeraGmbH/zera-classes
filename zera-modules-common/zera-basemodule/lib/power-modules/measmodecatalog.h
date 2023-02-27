#ifndef MEASMODECATALOG_H
#define MEASMODECATALOG_H

#include "measmodeinfo.h"
#include <QHash>

class MeasModeCatalog
{
public:
    static const cMeasModeInfo *getInfoSafe(QString name);
    static const cMeasModeInfo *getInfo(QString name);
private:
    static QHash<QString,cMeasModeInfo> m_modeInfoHash;
};

#endif // MEASMODECATALOG_H

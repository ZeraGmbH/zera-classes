#ifndef DSPMEASDATA_H
#define DSPMEASDATA_H

#include <QString>
#include <QList>
#include <QVector>

#include "dspvar.h"


class cDspMeasData
{
public:
    cDspMeasData(QString name, DSPDATA::DspSegType st = DSPDATA::sGlobal); // name des messdaten satzes
    ~cDspMeasData();
    float* data();
    void addVarItem(cDspVar*);
    quint32 getDataCount(); // anzahl der daten elemente in DspVarData
    QString& VarList(bool withType = false); // liste mit allen variablen mit längenangaben
    QString& MeasVarList(); // liste aller namen vom typ vdspresult
    QString& name();
    DSPDATA::DspSegType segType();

private:
    QList<cDspVar*> DspVarList;
    QVector<float> DspVarData;
    QString m_sname;
    DSPDATA::DspSegType m_SegmentType;
    QString m_slist;
};

#endif // DSPMEASDATA_H

#ifndef DSPMEASDATA_H
#define DSPMEASDATA_H

#include <QString>
#include <QList>
#include <QVector>

#include "dspvar.h"


class cDspMeasData
{
public:
    cDspMeasData(QString name); // name des messdaten satzes
    ~cDspMeasData();
    float* data(QString name);
    void addVarItem(cDspVar*);
    quint32 getSize(); // number of all element in this container
    quint32 getSize(QString name); // number of elements of var name in this container
    quint32 getumemSize();
    QString& VarList(int section, bool withType = false); // liste mit allen variablen mit längenangaben, optional mit data type
    QString& name();
    QString& writeCommand();
    QVector<float>& getData();


private:
    QList<cDspVar*> DspVarList;
    QVector<float> vector;
    QString m_sname;
    QString sReturn;
};

#endif // DSPMEASDATA_H

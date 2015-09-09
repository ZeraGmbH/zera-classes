#ifndef SCPIMEASUREDELEGATE_H
#define SCPIMEASUREDELEGATE_H

#include <QObject>
#include <QList>

#include "scpidelegate.h"

class QString;
class cSCPI;

namespace SCPIMODULE
{

class cSCPIMeasure;

class cSCPIMeasureDelegate: public cSCPIDelegate
{
   Q_OBJECT

public:
    cSCPIMeasureDelegate(QString cmdParent, QString cmd, quint8 type, quint8 measCode, cSCPIMeasure* scpimeasureobject);

    virtual bool executeSCPI(cSCPIClient *client, QString& sInput);
    void addscpimeasureObject(cSCPIMeasure* measureobject);

private:
    quint8 m_nMeasCode;
    QList<cSCPIMeasure*> m_scpimeasureObjectList;

};

}

#endif // SCPIMEASUREDELEGATE_H

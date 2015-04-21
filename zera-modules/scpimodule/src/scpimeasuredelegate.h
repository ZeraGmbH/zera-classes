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
    cSCPIMeasureDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode, cSCPIMeasure* scpimeasureobject);

    void addscpimeasureObject(cSCPIMeasure* measureobject);
    QList<cSCPIMeasure*>* getscpimeasureObjectList();

private:
    QList<cSCPIMeasure*> m_scpimeasureObjectList;

};

}

#endif // SCPIMEASUREDELEGATE_H

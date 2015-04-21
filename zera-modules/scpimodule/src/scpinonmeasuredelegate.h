#ifndef SCPINONMEASUREDELEGATE_H
#define SCPINONMEASUREDELEGATE_H

#include "scpidelegate.h"

class VeinPeer;
class VeinEntity;

namespace SCPIMODULE
{

class cSCPInonMeasureDelegate: public cSCPIDelegate
{
    Q_OBJECT

 public:
     cSCPInonMeasureDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode, VeinPeer* peer, VeinEntity* entity);
     VeinPeer* getPeer();
     VeinEntity* getEntity();

 private:
     VeinPeer* m_pPeer;
     VeinEntity* m_pEntity;
};

}
#endif // SCPINONMEASUREDELEGATE_H

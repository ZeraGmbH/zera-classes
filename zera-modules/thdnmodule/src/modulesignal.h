#ifndef MODULESIGNAL_H
#define MODULESIGNAL_H

#include <QObject>

class VeinEntity;
class VeinPeer;

// we use this class if we need an interface entity for signals

namespace THDNMODULE
{

class cModuleSignal: public QObject
{
    Q_OBJECT
public:
    cModuleSignal(VeinPeer *peer,QString name, QVariant initval);
    ~cModuleSignal();

    VeinPeer* m_pPeer;
    VeinEntity *m_pParEntity;
};

}

#endif // MODULESIGNAL_H

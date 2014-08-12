#ifndef MODULEINFO_H
#define MODULEINFO_H

#include <QObject>

class VeinEntity;
class VeinPeer;

// we use this class if we need an interface entity for information

namespace RANGEMODULE
{

class cModuleInfo: public QObject
{
    Q_OBJECT
public:
    cModuleInfo(VeinPeer *peer,QString name, QVariant initval);
    ~cModuleInfo();

private:
    VeinPeer* m_pPeer;
    VeinEntity *m_pParEntity;
};

}

#endif // MODULEINFO_H

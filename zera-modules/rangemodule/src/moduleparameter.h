#ifndef MODULEPARAMETER_H
#define MODULEPARAMETER_H

#include <QObject>
#include <QString>
#include <QVariant>

class VeinEntity;
class VeinPeer;

// we use this class if we need an interface entity for parameter for input/output incl. description

namespace RANGEMODULE
{

class cModuleParameter: public QObject
{
    Q_OBJECT
public:
    cModuleParameter(VeinPeer* peer,QString name, QVariant initval, QString limitname, QVariant limitval, bool readonly = false);
    ~cModuleParameter();
    void setData(QVariant val);
    QVariant getData();

signals:
    void updated(QVariant);

private:
    VeinPeer* m_pPeer;
    VeinEntity *m_pParEntity;
    VeinEntity *m_pLimitEntity;
};

}

#endif // MODULEPARAMETER_H

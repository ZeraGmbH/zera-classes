#ifndef MODULEPARAMETER_H
#define MODULEPARAMETER_H

#include <QObject>
#include <QString>
#include <QVariant>

class VeinEntity;
class VeinPeer;
class cParamValidator;

namespace VeinComponent
{
    class ComponentData;
}


// we use this class if we need an interface entity for parameter for input/output incl. description

class cModuleParameter: public QObject
{
    Q_OBJECT
public:
    cModuleParameter(VeinPeer* peer,QString name, QVariant initval, bool readonly = false, cParamValidator *pValidator = 0);
    ~cModuleParameter();
    void setData(QVariant val);
    QVariant getData();
    QString getName();
    bool paramTransaction(VeinComponent::ComponentData *cdata); // returns true if cdata belongs to this parameter

signals:
    void updated(QVariant);
    void updated(VeinComponent::ComponentData*);

private:
    VeinPeer* m_pPeer;
    QString m_sName;
    VeinEntity *m_pParEntity;
    cParamValidator *m_pParamValidator;
};

#endif // MODULEPARAMETER_H

#ifndef MODULEVALIDATOR_H
#define MODULEVALIDATOR_H

#include <QObject>
#include <QHash>
#include <QList>
#include <commandvalidator.h>

class cBaseModule;
class cVeinModuleParameter;

class cModuleValidator: public Zera::CommandValidator
{
    Q_OBJECT
public:
    cModuleValidator(cBaseModule* module);
    virtual ~cModuleValidator(){}

    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent);
    void setParameterHash(QHash<QString, cVeinModuleParameter *>&parameterhash);

private:
    cBaseModule* m_pModule;
    QHash<QString, cVeinModuleParameter*> m_Parameter2ValidateHash;
    QList<QString> m_keylist;
};

#endif // MODULEVALIDATOR_H

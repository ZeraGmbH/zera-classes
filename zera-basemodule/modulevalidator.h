#ifndef MODULEVALIDATOR_H
#define MODULEVALIDATOR_H

#include <QObject>
#include <QHash>

#include "commandvalidator.h"
#include "moduleparameter.h"



class cModuleValidator: public Zera::CommandValidator
{
    Q_OBJECT
public:
    cModuleValidator(QObject *t_parent = 0);

    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent)=0;
    QHash<QString, cModuleParameter*>& getParameter2ValidateHash();

private:
    QHash<QString, cModuleParameter*> m_Parameter2ValidateHash;
};

#endif // MODULEVALIDATOR_H

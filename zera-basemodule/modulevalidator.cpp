#include "modulevalidator.h"


cModuleValidator::cModuleValidator(QObject *t_parent)
    :Zera::CommandValidator(t_parent)
{
}


void cModuleValidator::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
{

}


QHash<QString, cModuleParameter *> &cModuleValidator::getParameter2ValidateHash()
{
    return m_Parameter2ValidateHash;
}

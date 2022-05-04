#ifndef ZERA_COMPONENTVALIDATOR_H
#define ZERA_COMPONENTVALIDATOR_H

#include "zera-validator_global.h"
#include <ve_eventsystem.h>
#include <ve_commandevent.h>

namespace Zera
{
/**
   * @brief Spezialized form of EventSystem that validates events of the type CommandEvent
   */
class ZERAVALIDATORSHARED_EXPORT CommandValidator : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    explicit CommandValidator(QObject *t_parent = 0);
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent)=0;
    bool processEvent(QEvent *t_event) override final;
};
} // namespace Zera

#endif // ZERA_COMPONENTVALIDATOR_H

#ifndef ZERA_BASEVALIDATOR_H
#define ZERA_BASEVALIDATOR_H

#include "zeravalidator_export.h"
#include <ve_eventsystem.h>

namespace Zera
{
  class CommandValidator;

  class ZERAVALIDATOR_EXPORT GroupValidator : public VeinEvent::EventSystem
  {
    Q_OBJECT
  public:
    GroupValidator(QObject *t_parent=0);

    void insertEntityValidator(int t_entityId, CommandValidator *t_cValidator);
    void removeEntityValidator(int t_entityId);
    const QHash<int, CommandValidator *> getEntityValidatorsCopy() const;


    void setGeneralValidators(const QList<CommandValidator *> &t_vList);

    // EventSystem interface
  public:
    bool processEvent(QEvent *t_event) override;

  private: /// @todo D'Pointer
    QList<CommandValidator *> m_generalValidators;
    QHash<int, CommandValidator *> m_entityValidators;
  };
} // namespace Zera

#endif // ZERA_BASEVALIDATOR_H

#ifndef APIMODULEAUTHORIZE_H
#define APIMODULEAUTHORIZE_H

#include <QObject>
#include "moduleactivist.h"
#include "apimodule.h"

namespace APIMODULE
{
class cApiModuleAuthorize : public cModuleActivist
{
public:
    cApiModuleAuthorize(cApiModule *module);
public slots:
    void generateVeinInterface() override;
    void activate() override;
    void deactivate() override;

private:
    cApiModule* m_module;

    VfModuleParameter* m_pPendingRequestPar;
    VfModuleParameter* m_pRequestStatusAct;
    VfModuleParameter* m_pTrustListAct;
    VfModuleParameter* m_pTrustListChangeCountAct;
};
}

#endif // APIMODULEAUTHORIZE_H

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
    QJsonArray readTrustList();
    bool jsonArrayContains(const QJsonArray& array, const QJsonObject& target);

    const QString m_trustListPath = "/opt/websam-vein-api/authorize/trustlist.json";
    cApiModule* m_module;

    VfModuleParameter* m_pPendingRequestPar;
    VfModuleParameter* m_pGuiDialogFinished;
    VfModuleComponent* m_pPendingRequestAct;
    VfModuleComponent* m_pRequestStatusAct;
    VfModuleComponent* m_pTrustListAct;
    VfModuleComponent* m_pTrustListChangeCountAct;

private slots:
    void onNewPendingRequest(QVariant pendingRequest);
    void onGuiDialogFinished(QVariant dialogFinished);
};
}

#endif // APIMODULEAUTHORIZE_H

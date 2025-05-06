#ifndef APIMODULEAUTHORIZE_H
#define APIMODULEAUTHORIZE_H

#include <QObject>
#include "moduleactivist.h"
#include "apimodule.h"
#include <vf-cpp-rpc.h>
#include <quuid.h>

namespace APIMODULE
{
class cApiModuleAuthorize : public cModuleActivist
{
    Q_OBJECT

public:
    cApiModuleAuthorize(cApiModule *module);
public slots:
    void generateVeinInterface() override;
    void activate() override;
    void deactivate() override;

    QVariant RPC_Authenticate(QVariantMap p_params);

private:
    QJsonArray readTrustList();
    bool jsonArrayContains(const QJsonArray& array, const QJsonObject& target);

    const QString m_trustListPath = "/opt/websam-vein-api/authorize/trustlist.json";

    cApiModule* m_module;
    QUuid m_rpcRequest;

    VfModuleParameter* m_pGuiDialogFinished;
    VfModuleComponent* m_pPendingRequestAct;
    VfModuleComponent* m_pRequestStatusAct;
    VfModuleComponent* m_pTrustListAct;
    VfModuleComponent* m_pTrustListChangeCountAct;

    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcAuthenticateInterface;

signals:
    void finishDialog(QVariant dialogFinished);
    void rejectRpc(QUuid uuid);

private slots:
    void onGuiDialogFinished(QVariant dialogFinished);
    void onRpcRejected(QUuid uuid);
};
}

#endif // APIMODULEAUTHORIZE_H

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
    cApiModuleAuthorize(cApiModule *module, QString persistencyBasePath);
public slots:
    void generateVeinInterface() override;
    void activate() override;
    void deactivate() override;

    QVariant RPC_Authenticate(QVariantMap p_params);
    void setTrustListPath(const QString &path);

private:
    QJsonArray readTrustList();
    bool jsonArrayContains(const QJsonArray& array, const QJsonObject& target);

    QString m_trustListPath = "/opt/websam-vein-api/authorize/trustlist.json";

    cApiModule* m_module;
    QUuid m_rpcRequest;

    VfModuleParameter* m_pGuiDialogFinished;
    VfModuleParameter* m_pReloadTrustList;

    std::shared_ptr<VfModuleComponent> m_spPendingRequestAct;
    std::shared_ptr<VfModuleComponent> m_spTrustListAct;
    std::shared_ptr<VfModuleComponent> m_spTrustListChangeCountAct;

    VfCpp::cVeinModuleRpc::Ptr m_spRpcAuthenticateInterface;

    bool m_bDialogIsOpen;

signals:
    void finishDialog(QVariant dialogFinished);
    void rejectRpc(QUuid uuid);

private slots:
    void onGuiDialogFinished(QVariant dialogFinished);
    void onRpcRejected(QUuid uuid);
    void onReloadTrustList(QVariant reload);
};
}

#endif // APIMODULEAUTHORIZE_H

#ifndef RPCCREATEVERSIONFILE_H
#define RPCCREATEVERSIONFILE_H

#include <vf-cpp-rpc-simplified.h>
#include <QUuid>
#include <QJsonObject>

class RPCCreateVersionFile: public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCCreateVersionFile(VeinEvent::EventSystem *eventSystem, int entityId, std::shared_ptr<QJsonObject> object);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
private:
    bool createVersionsFile(QString jsonPath);

    std::shared_ptr<QJsonObject> m_versionsObject;
};

#endif // RPCCREATEVERSIONFILE_H

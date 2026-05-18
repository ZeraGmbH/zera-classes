#include "rpccreateversionfile.h"
#include <vf-cpp-rpc-signature.h>
#include <QJsonDocument>
#include <QDir>

RPCCreateVersionFile::RPCCreateVersionFile(VeinEvent::EventSystem *eventSystem, int entityId, std::shared_ptr<QJsonObject> object) :
    m_versionsObject(object),
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_CreateVersionFile",
                                  VfCpp::VfCppRpcSignature::RPCParams({{"p_destinationPath", "QString"}})))
{
}

void RPCCreateVersionFile::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString path = parameters["p_destinationPath"].toString();
    if(createVersionsFile(path))
        sendRpcResult(callId, QByteArray("Version file created successfully"));
    else
        sendRpcError(callId, "Version file not created");
}

bool RPCCreateVersionFile::createVersionsFile(QString jsonPath)
{
    bool ok = false;
    if(m_versionsObject) {
        QJsonDocument jsonDoc(*m_versionsObject);
        QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);
        QFile jsonFile(jsonPath);
        if (jsonFile.open(QIODevice::WriteOnly)) {
            jsonFile.write(jsonData);
            jsonFile.close();
            ok = true;
        }
    }
    return ok;
}

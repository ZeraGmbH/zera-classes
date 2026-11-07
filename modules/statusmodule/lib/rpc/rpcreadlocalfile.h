#ifndef RPCREADLOCALFILE_H
#define RPCREADLOCALFILE_H

#include <vf-cpp-rpc-simplified.h>

class RpcReadLocalFile : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RpcReadLocalFile(VeinEvent::EventSystem *eventSystem, int entityId);

    static void setFileDirBasePath(const QString &path);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
private:
    static QString m_fileDirBasePath;
};

#endif // RPCREADLOCALFILE_H

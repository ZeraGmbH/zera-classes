#ifndef RPCLISTLOCALFILES_H
#define RPCLISTLOCALFILES_H

#include <vf-cpp-rpc-simplified.h>

class RpcListLocalFiles : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RpcListLocalFiles(VeinEvent::EventSystem *eventSystem, int entityId);

    // For now static (for tests)
    static void setFileDirBasePath(const QString &path);
    static void setMaxListedFiles(int maxListedFiles);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
private:
    static QString m_fileDirBasePath;
    static int m_maxListedFiles;
};

#endif // RPCLISTLOCALFILES_H

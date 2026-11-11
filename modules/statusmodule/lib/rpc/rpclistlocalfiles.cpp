#include "rpclistlocalfiles.h"
#include <vf-cpp-rpc-signature.h>
#include <QDir>
#include <QDirIterator>

static constexpr int kDefaultMaxListedFiles = 1000;

QString RpcListLocalFiles::m_fileDirBasePath = SCPI_LOCAL_FILE_PATH;
int RpcListLocalFiles::m_maxListedFiles = kDefaultMaxListedFiles;

RpcListLocalFiles::RpcListLocalFiles(VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_ListLocalFiles",
                                  VfCpp::VfCppRpcSignature::RPCParams()))
{
}

void RpcListLocalFiles::setFileDirBasePath(const QString &path)
{
    m_fileDirBasePath = path;
}

void RpcListLocalFiles::setMaxListedFiles(int maxListedFiles)
{
    m_maxListedFiles = maxListedFiles;
}

void RpcListLocalFiles::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    Q_UNUSED(parameters)
    QString strReturned;
    if (!QDir(m_fileDirBasePath).exists())
        strReturned = "ERROR: file directory not found";
    else {
        QStringList fileNames;
        QDirIterator it(m_fileDirBasePath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        for (int i=0; it.hasNext() && i<m_maxListedFiles; ++i)
            fileNames.append(QDir(m_fileDirBasePath).relativeFilePath(it.next()));
        fileNames.sort();
        strReturned = fileNames.join("\n");
    }
    // Decision: Return error message and do not go the RPC/SCPI error way
    sendRpcResult(callId, strReturned);
}

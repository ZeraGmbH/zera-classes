#include "rpcreadlocalfile.h"
#include <safefileloader.h>
#include <textencodingconverter.h>
#include <vf-cpp-rpc-signature.h>

QString RpcReadLocalFile::m_fileDirBasePath = SCPI_LOCAL_FILE_PATH;

RpcReadLocalFile::RpcReadLocalFile(VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_ReadLocalFile",
                                  VfCpp::VfCppRpcSignature::RPCParams({{"p_fileName", "QString"}})))
{
}

void RpcReadLocalFile::setFileDirBasePath(const QString &path)
{
    m_fileDirBasePath = path;
}

void RpcReadLocalFile::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString filePath = parameters["p_fileName"].toString();
    QString strReturned;
    if (!filePath.isEmpty()) {
        QByteArray fileData;
        SafeFileLoader::LoadResult loadResult = SafeFileLoader::readFile(fileData, filePath, m_fileDirBasePath);
        if (loadResult == SafeFileLoader::Ok) {
            TextEncodingConverter::Result convertResult = TextEncodingConverter::convert(fileData);
            if (convertResult.ok)
                strReturned = convertResult.text;
            else
                strReturned = "ERROR: File text conversion failed";
        }
        else
            strReturned = SafeFileLoader::stringifyLoadResult(loadResult);
    }
    else
        strReturned = "ERROR: missing parameter'";
    // Decision: Return error message and do not go the RPC/SCPI error way
    sendRpcResult(callId, strReturned);
}

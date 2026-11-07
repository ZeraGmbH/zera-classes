#include "scpimodellocalfile.h"
#include "scpilocalfileread.h"
#include "scpilocalfileerrormessages.h"
#include "scpimodule.h"
#include "scpidelegateinterface.h"
#include <zscpi_response_definitions.h>

namespace SCPIMODULE
{

enum scpiinterfacecommands
{
    localfilecmd
};

// on target the files live in the operator home - for tests this can be overridden
// by setting the environment variable ZERA_SCPI_LOCALFILE_BASEPATH
static constexpr const char* s_baseDirPathEnvVar = "ZERA_SCPI_LOCALFILE_BASEPATH";


ScpiModelLocalFile::ScpiModelLocalFile(cSCPIModule *module) :
    m_pModule(module)
{
}

QString ScpiModelLocalFile::getBaseDirPath()
{
    return qEnvironmentVariable(s_baseDirPathEnvVar, SCPI_LOCAL_FILE_PATH);
}

void ScpiModelLocalFile::setupScpi(cSCPIInterface *scpiInterface)
{
    cSCPIInterfaceDelegatePtr delegate = std::make_shared<ScpiDelegateInterface>(
                ScpiDelegateInterface::Params{"SYSTEM", "LOCALFILE", SCPI::isQuery | SCPI::isCmdwP, localfilecmd,
                                              QString("Returns the content of a text file (UTF-8) from the local file system: SYSTEM:LOCALFILE? <filepath>;")});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelLocalFile::executeCmd);

    // for module integrity we also have to add this command to the scpi command list (exported at INF_ModuleInterface)
    m_pModule->scpiCommandList.append(new VfModuleMetaInfoContainer("", QString("SYSTEM:LOCALFILE"), SCPI::isQuery | SCPI::isCmdwP, ""));
}

void ScpiModelLocalFile::executeCmd(cSCPIClient *client,
                                    int cmdCode,
                                    const QString &scpi,
                                    const ScpiTransactionId &scpiTransactionId)
{
    Q_UNUSED(cmdCode)

    // we only support the query form - the command form (e.g. SYSTEM:LOCALFILE <par>;) is rejected
    if (!scpi.contains('?')) {
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
        return;
    }

    const QString filePath = extractFilePathParam(scpi);
    if (filePath.isEmpty())
        client->handleCmdFinish(ScpiLocalFileErrorMessages::ScpiParameterMissing, scpiTransactionId);
    else if (filePath.contains(';'))
        client->handleCmdFinish(ScpiLocalFileErrorMessages::ScpiTooManyParameters, scpiTransactionId);
    else
        client->handleCmdFinish(ScpiLocalFileRead::readLocalFileContent(getBaseDirPath(), filePath), scpiTransactionId);
}

QString ScpiModelLocalFile::extractFilePathParam(const QString &scpi)
{
    // the file path is everything behind the '?' - both 'SYSTEM:LOCALFILE? <par>;' and
    // 'SYSTEM:LOCALFILE?<par>;' are allowed (also without trailing ';')
    const int queryIndex = scpi.indexOf('?');
    if (queryIndex < 0)
        return QString();
    QString filePath = scpi.mid(queryIndex + 1);

    // remove the terminating ';' and surrounding whitespaces
    if (filePath.endsWith(';'))
        filePath.chop(1);
    filePath = filePath.trimmed();

    return filePath;
}

bool ScpiModelLocalFile::isValidUtf8(const QByteArray &data)
{
    const auto *p = reinterpret_cast<const unsigned char*>(data.constData());
    const int n = data.size();
    int i = 0;
    while (i < n) {
        const unsigned char c = p[i];
        int trailingBytes;
        if (c <= 0x7F)
            trailingBytes = 0;
        else if ((c & 0xE0) == 0xC0) {
            trailingBytes = 1;
            if (c < 0xC2) // overlong encoding of ascii character
                return false;
        }
        else if ((c & 0xF0) == 0xE0) {
            trailingBytes = 2;
            if (c == 0xE0 && i + 1 < n && p[i + 1] < 0xA0) // overlong encoding of U+0080..U+07FF
                return false;
        }
        else if ((c & 0xF8) == 0xF0) {
            trailingBytes = 3;
            if (c > 0xF4) // above U+10FFFF
                return false;
            if (c == 0xF0 && i + 1 < n && p[i + 1] < 0x90) // overlong encoding of U+1000..U+3FFF
                return false;
        }
        else
            return false; // continuation byte without leading byte or invalid leading byte

        for (int j = 1; j <= trailingBytes; ++j) {
            if (i + j >= n || (p[i + j] & 0xC0) != 0x80)
                return false;
        }
        i += trailingBytes + 1;
    }
    return true;
}

}

#ifndef SCPIMODELLOCALFILE_H
#define SCPIMODELLOCALFILE_H

#include "scpimodelbase.h"
#include "scpiinterface.h"

namespace SCPIMODULE
{

class cSCPIModule;

/* Provides the SCPI query SYSTEM:LOCALFILE? <filepath> which returns the content
 * of a text file (UTF-8) from the local file system. The file path is always
 * resolved relative to a base directory so no file outside of it can be read.
 */
class ScpiModelLocalFile : public ScpiModelBase
{
    Q_OBJECT
public:
    explicit ScpiModelLocalFile(cSCPIModule* module);
    void setupScpi(cSCPIInterface *scpiInterface);

    // static functions so they can be unit tested without a running session
    static QString getBaseDirPath();
    static bool isValidUtf8(const QByteArray &data);
    static QString extractFilePathParam(const QString &scpi);

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client,
                    int cmdCode,
                    const QString& scpi,
                    const ScpiTransactionId &scpiTransactionId);
private:
    cSCPIModule* m_pModule = nullptr;
};

}

#endif // SCPIMODELLOCALFILE_H

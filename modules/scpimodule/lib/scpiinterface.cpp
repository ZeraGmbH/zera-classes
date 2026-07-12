#include <scpi.h>
#include <scpiobject.h>
#include <scpicommand.h>
#include <QString>

#include "scpiinterface.h"
#include "scpiclient.h"
#include "scpidelegatetemplate.h"


namespace SCPIMODULE
{

cSCPIInterface::cSCPIInterface(const QString &name) :
    m_sName(name)
{
}

void cSCPIInterface::exportSCPIModelXML(QString &xml, QMap<QString, QString> modelListBaseEntry)
{
    modelListBaseEntry.insert(modelListBaseEntry.constBegin(), "DEVICE", m_sName);
    m_scpiCmdInterface.exportSCPIModelXML(xml, modelListBaseEntry);
}

void cSCPIInterface::addSCPICommand(ScpiBaseDelegatePtr delegate)
{
    m_scpiCmdInterface.insertScpiCmd(delegate->getCmdParent(), delegate);
}

bool cSCPIInterface::executeCmd(cSCPIClient *client, const QString &cmd, const ScpiTransactionId &scpiTransactionId)
{
    ScpiObjectPtr scpiObject = m_scpiCmdInterface.getSCPIObject(cmd);
    if (scpiObject != nullptr) {
        ScpiDelegateTemplate* scpiDelegate = static_cast<ScpiDelegateTemplate*>(scpiObject.get());
        scpiDelegate->executeSCPI(client, cmd, scpiTransactionId);
        return true;
    }
    return false;
}

void cSCPIInterface::checkAmbiguousShortNames()
{
    ScpiAmbiguityMap ambiguityMap = m_scpiCmdInterface.checkAmbiguousShortNames(ignoreAmbiguous);
    if(!ambiguityMap.isEmpty()) {
        for(auto iter=ambiguityMap.constBegin(); iter!=ambiguityMap.constEnd(); ++iter) {
            qWarning("SCPI-module: Ambiguous short %s for %s",
                     qPrintable(iter.key()),
                     qPrintable(iter.value().join(" / ")));
        }
        Q_ASSERT(false);
    }
}

ScpiAmbiguityMap cSCPIInterface::ignoreAmbiguous(ScpiAmbiguityMap inMap)
{
    ScpiAmbiguityMap outMap;

    for(auto iter = inMap.constBegin(); iter != inMap.constEnd(); ++iter) {
        bool ignore = false;

        static QStringList ignoreEndShortList = QStringList() <<
                                                ":DFT1:UL3-" <<
                                                ":RMS1:UL3-" <<
                                                ":LAM1:LAMB" <<
                                                ":EC01:0001:RES" <<
                                                ":EC01:0002:RES" <<
                                                "CONF:EC01:0001:DUTC" <<
                                                "CONF:EC01:0002:DUTC" <<
                                                ":FFT1:IAUX" <<
                                                ":FFT1:UAUX" <<
                                                "STAT:DEV1:ACC";
        for(const auto &ignoreEndShort : qAsConst(ignoreEndShortList)) {
            if(iter.key().endsWith(ignoreEndShort))
                ignore = true;
        }

        if(!ignore)
            outMap[iter.key()] = iter.value();
    }

    return outMap;
}

}

#ifndef SCPITESTCLIENT_H
#define SCPITESTCLIENT_H

#include "scpiclient.h"

namespace SCPIMODULE {

// Enhanced SCPI I/O designed to anlayse insights. If tests require just I/O, see ScpiModuleNetClientBlocked
class ScpiTestClient : public cSCPIClient
{
    Q_OBJECT
public:
    ScpiTestClient(cSCPIModule* module);
    ~ScpiTestClient() override;

    void handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;

    // Convenience blocked one transaction
    QString sendReceiveNotSorted(const QString &scpi, bool removeLineFeedOnReceive);
    QString sendReceiveSorted(const QString &scpi, bool removeLineFeedOnReceive);
    // Enhanced unblocked any number of transactions
    void sendScpiCmds(QString cmds);

    const NullableStringList &getResponsesNotSorted() const;
    const NullableStringList &getResponsesSorted() const;

    int getAllHandledResponseCount() const;
    int getUnhandledResponses() const;
    bool getAtLeastOneResponse() const;

    void clearResponses();
signals:
    void sigScpiResponseNotSorted(const QString &scpiResponse, bool isNull, const QString &scpi);
    void sigScpiResponseSorted(const QString &scpiResponse, bool isNull, const QString &scpi);

private slots:
    void cmdInput() override;
private:
    QString responseToStr(const NullableString &response, bool removeLineFeedOnReceive);

    NullableStringList m_responseNotSorted;
    NullableStringList m_responsesSorted;
    int m_allResponsesReceivedNotClearable = 0;
    int m_allResponsesPending = 0;
};

}

#endif // SCPITESTCLIENT_H

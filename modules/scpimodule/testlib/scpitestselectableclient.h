#ifndef SCPITESTSELECTABLECLIENT_H
#define SCPITESTSELECTABLECLIENT_H

#include "scpimodulenetclientblocked.h"
#include "scpitestclient.h"

class ScpiTestSelectableClient : public QObject
{
    Q_OBJECT
public:
    enum ClientType {
        NET,
        TEST
    };
    Q_ENUM(ClientType)

    explicit ScpiTestSelectableClient(ClientType clientType, SCPIMODULE::cSCPIModule* scpiModule);
    QString sendReceive(const QString &scpi);

private slots:
    void onTestScpiAnswer(const QString &scpiResponse);
private:
    ClientType m_clientType;
    std::unique_ptr<ScpiModuleNetClientBlocked> m_blockedClient;
    std::unique_ptr<SCPIMODULE::ScpiTestClient> m_testClient;
    QString m_testClientResponse;
};

#endif // SCPITESTSELECTABLECLIENT_H

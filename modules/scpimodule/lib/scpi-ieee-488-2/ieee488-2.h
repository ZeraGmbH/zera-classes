#ifndef IEEE4882_H
#define IEEE4882_H

#include "scpitransactionid.h"
#include <vs_abstractdatabase.h>
#include <QObject>
#include <QString>
#include <QVector>

namespace SCPIMODULE
{

class cSCPIClient;
class cSCPIModule;

enum opcStates { OCIS, // operation idle state
                 OCAS, // operation active state
                 OQIS, // operation query idle state
                 OQAS }; // operation query active state

class cIEEE4882: public QObject
{
    Q_OBJECT
public:
    cIEEE4882(cSCPIClient* client,
              const QString &deviceFamilyFromConfig,
              VeinStorage::AbstractDatabase *storageDb);
    void executeCmd(cSCPIClient* client,
                    int cmdCode,
                    const QString &scpi,
                    const ScpiTransactionId &scpiTransactionId);
    static int getErrorQueueLength();
signals:
    void setQuestionableCondition(quint16);
    void setOperationCondition(quint16);
    void setOperationMeasureCondition(quint16);

public slots:
    void addEventErrorWithResponse(int error, const ScpiTransactionId &scpiTransactionId);
    void setStatusByte(quint8 stb, quint8); // we accept 16bit unless stb is 8 bit, but scpi status system uses 16 bit

private:
    cSCPIClient* m_pClient;
    QString m_deviceFamilyFromConfig;
    const VeinStorage::AbstractComponentPtr m_serNoComponent;
    QVector<int> m_ErrEventQueue;

    QString getIdentification();
    QString stringifyRegisterForOutput(quint8 reg);
    QString popScpiErrorAndMakeString();

    void ResetDevice();
    void ClearStatus();
    void ClearEventError();
    void SetnoOperFlag(bool b);

    // using this functions for setting/resetting ieee488-1 register will cause additional actions
    void SetSTB(quint8 b); // set STB status
    void SetSRE(quint8 b); // set SRE service request enable register
    void SetESR(quint8 b); // set ESR standard event status register
    void SetESE(quint8 b); // set ESE event status enable register

    quint8 m_nSTB; // status byte !!!!!! setting/resetting these 4 registers must be done using dedicated functions
    quint8 m_nSRE; // service reguest enable
    quint8 m_nESR; // standard event status register
    quint8 m_nESE; // standard event status enable
    opcStates m_nOPCState;
    opcStates m_nOPCQState;

    bool m_bnoOperationPendingFlag; // use dedicated function for setting/resetting
};

}
#endif // IEEE4882_H

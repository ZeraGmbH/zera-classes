#ifndef IEEE4882_H
#define IEEE4882_H

#include <QObject>
#include <QString>
#include <QVector>

namespace SCPIMODULE
{

enum scpicommoncommands
{
    operationComplete,
    eventstatusenable,
    servicerequestenable,
    clearstatus,
    reset,
    identification,
    eventstatusregister,
    statusbyte,
    selftest,
    read1error,
    readerrorcount,
    readallerrors
};

enum scpiErrorInd
{
    NoError,
    CommandError,
    InvalidSeparator,
    ParameterNotAllowed,
    MissingParameter,
    UndefinedHeader,
    NumericDataError,

    ExecutionError,
    CommandProtected,
    HardwareError,
    HardwareMissing,
    FileNameError,

    DeviceSpecificError,
    systemError,
    QueueOverflow,
    QueryError,
    PowerOn,
    scpiLastError
};


struct scpiErrorType
{
    short ErrNum; // fehler nummer (negativ)
    char* ErrTxt; // fehler text
};




enum STBBits
{   STBbit0,
    STBbit1,
    STBeeQueueNotEmpty,
    STBques,
    STBmav,
    STBesb,
    STBrqs,
    STBoper
};


enum SESRBits
{
    SESROperationComplete = 1,
    SESRRequestControl = 2,
    SESRQueryError = 4,
    SESRDevDepError = 8,
    SESRExecError = 16,
    SESRCommandError = 32,
    SESRUserRequest = 64,
    SESRPowerOn = 128
};


enum opcStates { OCIS, // operation idle state
                 OCAS, // operation active state
                 OQIS, // operation query idle state
                 OQAS }; // operation query active state


class cSCPIClient;
class cSCPIModule;

class cIEEE4882: public QObject
{
    Q_OBJECT

public:
    cIEEE4882(cSCPIClient* client, QString ident, quint16 errorqueuelen);

    void executeCmd(cSCPIClient* client, int cmdCode, const QString &sInput);

signals:
    void setQuestionableCondition(quint16);
    void setOperationCondition(quint16);
    void setOperationMeasureCondition(quint16);

public slots:
    void AddEventError(int error); // if any error occurs from scpi commands we report them here
    void setStatusByte(quint8 stb, quint8); // we accept 16bit unless stb is 8 bit, but scpi status system uses 16 bit

private:
    cSCPIClient* m_pClient;
    QString m_sIdentification;
    quint16 m_nQueueLen; // max. entries in m_ErrEventQueue

    QVector<int> m_ErrEventQueue;

    void setIdentification(QString ident); // ident: default from <device>-scpimodule.xml
    QString RegOutput(quint8 reg);
    QString mGetScpiError();

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

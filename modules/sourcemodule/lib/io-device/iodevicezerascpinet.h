#ifndef IODEVICEZERASCPINET_H
#define IODEVICEZERASCPINET_H

#include "iodevicebase.h"
#include <pcbinterface.h>
#include <timertemplateqt.h>

class IoDeviceZeraSCPINet : public IoDeviceBase
{
    Q_OBJECT
public:
    IoDeviceZeraSCPINet(Zera::PcbInterfacePtr pcbInterface);
    bool open(QString strDeviceInfo) override;
    void close() override;
    bool isOpen() override;
    int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) override;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer);
    void onTimeout();
private:
    void setReadTimeoutNextIo(int timeoutMs) override;
    void errorOut(IoTransferDataSingle::EvalResponse eval, const QByteArray &response);

    Zera::PcbInterfacePtr m_pcbInterface;
    bool m_isOpen = false;
    int m_timeoutMs = TRANSACTION_TIMEOUT;
    TimerTemplateQtPtr m_timeoutTimer;
};

#endif // IODEVICEZERASCPINET_H

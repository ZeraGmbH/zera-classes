#ifndef CSOURCEINTERFACEZERASERIAL_H
#define CSOURCEINTERFACEZERASERIAL_H

#include <QSerialPortAsyncBlock>
#include "sourceinterface.h"

class cSourceInterfaceZeraSerialPrivate;

class cSourceInterfaceZeraSerial : public cSourceInterfaceBase
{
    Q_OBJECT
public:
    explicit cSourceInterfaceZeraSerial(QObject *parent = nullptr);
    // see https://github.com/ZeraGmbH/qtiohelper / QT += serialportasyncblock
    bool open(QString portName); // e.g "ttyUSB0" not "/dev/ttyUSB0" !!
    virtual int sendAndReceive(QByteArray dataSend, QByteArray* pDataReceive) override;
    void setReadTimeoutNextIo(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal = 0);
    void setBlockEndCriteriaNextIo(int iBlockLenReceive = 0, QByteArray endBlock = QByteArray());

signals:

private slots:
    void onIoFinished();

private:
    cSourceInterfaceZeraSerialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(cSourceInterfaceZeraSerial)

    QSerialPortAsyncBlock m_serialIO;
    int m_currentTransactionID = 0;
};

#endif // CSOURCEINTERFACEZERASERIAL_H

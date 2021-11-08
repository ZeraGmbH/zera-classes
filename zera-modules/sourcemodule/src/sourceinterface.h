#ifndef SOURCEINTERFACEBASE_H
#define SOURCEINTERFACEBASE_H

#include <QObject>
#include <QSerialPortAsyncBlock>

// ------------------------- helper classes --------------------------
class cSourceInterfaceTransactionIdGenerator
{
public:
    /**
     * @brief nextTransactionID
     * @return unique value != 0
     */
    int nextTransactionID();
private:
    int m_currentTransActionID = -1;
};

// available
enum SourceInterfaceType
{
    SOURCE_INTERFACE_BASE = 0,
    SOURCE_INTERFACE_DEMO,
    SOURCE_INTERFACE_ASYNCSERIAL,

    SOURCE_INTERFACE_TYPE_COUNT
};

class cSourceInterfaceBase;
// create interfaces through cSourceInterfaceFactory only
class cSourceInterfaceFactory {
public:
    static cSourceInterfaceBase* createSourceInterface(SourceInterfaceType type, QObject *parent = nullptr);
};


// ------------------------- base interface --------------------------
/**
 * @brief cSourceInterfaceBase: Interface + make sure ioFinished is queued
 */
class cSourceInterfaceBase : public QObject
{
    Q_OBJECT
public:
    virtual SourceInterfaceType type() { return SOURCE_INTERFACE_BASE; }
    virtual bool open(QString) { return false; /* cannot open base */}
    virtual void close();
    /**
     * @brief sendAndReceive
     * @param dataSend
     * @param pDataReceive
     * @return != 0: transactionID / = 0: transactionID not started
     */
    virtual int sendAndReceive(QByteArray dataSend, QByteArray* pDataReceive);
signals:
    void sigDisconnected(cSourceInterfaceBase *ioInterface);
    void ioFinished(int transactionID); // users connect this signal
    void ioFinishedToQueue(int transactionID); // sub classes emit this to ensure queue

protected:
    explicit cSourceInterfaceBase(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;

    cSourceInterfaceTransactionIdGenerator m_transactionIDGenerator;
};


// ------------------------- demo interface --------------------------
/**
 * @brief cSourceInterfaceDemo: Emit ioFinished
 */
class cSourceInterfaceDemo : public cSourceInterfaceBase
{
    Q_OBJECT
public:
    virtual SourceInterfaceType type() override { return SOURCE_INTERFACE_DEMO; }
    virtual bool open(QString) override { return true; }
    virtual int sendAndReceive(QByteArray dataSend, QByteArray* pDataReceive) override;
    void simulateExternalDisconnect();
protected:
    explicit cSourceInterfaceDemo(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;
};


// ------------------------- ZERA serial interface --------------------------
/**
 * @brief cSourceInterfaceDemo: Zera serial IO
 */
class cSourceInterfaceZeraSerialPrivate;
class cSourceInterfaceZeraSerial : public cSourceInterfaceBase
{
    Q_OBJECT
public:
    virtual SourceInterfaceType type() override { return SOURCE_INTERFACE_ASYNCSERIAL; }
    virtual int sendAndReceive(QByteArray dataSend, QByteArray* pDataReceive) override;
    virtual bool open(QString strDeviceInfo) override; // e.g "/dev/ttyUSB0"
    virtual void close() override;
    // wrappers - see https://github.com/ZeraGmbH/qtiohelper / QT += serialportasyncblock
    void setReadTimeoutNextIo(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal = 0);
    void setBlockEndCriteriaNextIo(int iBlockLenReceive = 0, QByteArray endBlock = QByteArray());
protected:
    explicit cSourceInterfaceZeraSerial(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;
private slots:
    void onIoFinished();
private:
    cSourceInterfaceZeraSerialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(cSourceInterfaceZeraSerial)

    QSerialPortAsyncBlock m_serialIO;
    int m_currentTransactionID = 0;
};

#endif // SOURCEINTERFACEBASE_H

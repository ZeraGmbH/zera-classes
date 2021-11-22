#ifndef SOURCEINTERFACEBASE_H
#define SOURCEINTERFACEBASE_H

#include <QObject>
#include <QSerialPortAsyncBlock>
#include <QSharedPointer>
#include <QTimer>
#include "sourceidgenerator.h"

enum SourceInterfaceTypes
{
    SOURCE_INTERFACE_BASE = 0,
    SOURCE_INTERFACE_DEMO,
    SOURCE_INTERFACE_ASYNCSERIAL,

    SOURCE_INTERFACE_TYPE_COUNT
};

class cSourceInterfaceBase;
typedef QSharedPointer<cSourceInterfaceBase> tSourceInterfaceShPtr;

// create interfaces through cSourceInterfaceFactory only
class cSourceInterfaceFactory {
public:
    static tSourceInterfaceShPtr createSourceInterface(SourceInterfaceTypes type, QObject *parent = nullptr);
};


// ------------------------- base interface --------------------------
/**
 * @brief cSourceInterfaceBase: Interface + make sure sigIoFinished is queued
 */
class cSourceInterfaceBase : public QObject
{
    Q_OBJECT
public:
    virtual ~cSourceInterfaceBase();
    virtual SourceInterfaceTypes type() { return SOURCE_INTERFACE_BASE; }
    bool isDemo() { return type() == SOURCE_INTERFACE_DEMO; }
    virtual bool open(QString) { return false; }
    virtual void close();
    /**
     * @brief sendAndReceive
     * @param bytesSend
     * @param pDataReceive
     * @return != 0: ioID / == 0: io not started
     */
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive);
signals:
    void sigDisconnected();
    void sigIoFinished(int ioID); // users connect this signal
    void sigIoFinishedToQueue(int ioID); // sub classes emit this to ensure queue

protected:
    explicit cSourceInterfaceBase(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;

    cSourceIdGenerator m_IDGenerator;
};


// ------------------------- demo interface --------------------------
/**
 * @brief cSourceInterfaceDemo: Emit sigIoFinished
 */
class cSourceInterfaceDemo : public cSourceInterfaceBase
{
    Q_OBJECT
public:
    virtual SourceInterfaceTypes type() override { return SOURCE_INTERFACE_DEMO; }
    virtual bool open(QString) override;
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive) override;
    void simulateExternalDisconnect();
    void setResponseDelay(int iMs);
    void setResponses(QList<QByteArray> responseList);
protected:
    explicit cSourceInterfaceDemo(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;
private slots:
    void onResponseDelayTimer();
private:
    void sendResponse();

    bool m_bOpen = false;
    int m_responseDelayMs = 0;
    QTimer m_responseDelayTimer;
    int m_currentId = 0;
    QByteArray* m_pDataReceive = nullptr;
    QList<QByteArray> m_responseList;
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
    virtual ~cSourceInterfaceZeraSerial();
    virtual SourceInterfaceTypes type() override { return SOURCE_INTERFACE_ASYNCSERIAL; }
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive) override;
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
    void onDeviceFileGone(QString);
private:
    cSourceInterfaceZeraSerialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(cSourceInterfaceZeraSerial)

    QSerialPortAsyncBlock m_serialIO;
    int m_currentIoID = 0;
};

#endif // SOURCEINTERFACEBASE_H

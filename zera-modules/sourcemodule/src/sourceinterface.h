#ifndef SOURCEINTERFACEBASE_H
#define SOURCEINTERFACEBASE_H

#include <QObject>
#include <QSerialPortAsyncBlock>
#include <QSharedPointer>
#include <QTimer>
#include "sourceidgenerator.h"

static constexpr int sourceDefaultTimeout = 1500;

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
class cSourceInterfaceBase : public QObject
{
    Q_OBJECT
public:
    virtual ~cSourceInterfaceBase();

    virtual bool open(QString) { return false; }
    virtual void close() {}
    virtual void setReadTimeoutNextIo(int) {};
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive);

    virtual bool isOpen() { return false; }
    bool isDemo() { return type() == SOURCE_INTERFACE_DEMO; }
    QString getDeviceInfo();
    virtual SourceInterfaceTypes type() { return SOURCE_INTERFACE_BASE; }

signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool ioError); // users connect this signal
    void sigIoFinishedToQueue(int ioID, bool ioError); // sub classes emit this to ensure queue

protected:
    explicit cSourceInterfaceBase(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;

    QString m_strDeviceInfo;
    cSourceIdGenerator m_IDGenerator;
    int m_currentIoID = 0;
};


// ------------------------- demo interface --------------------------
class cSourceInterfaceDemo : public cSourceInterfaceBase
{
    Q_OBJECT
public:
    virtual bool open(QString strDeviceInfo) override;
    virtual void close() override;
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive) override;
    virtual void setReadTimeoutNextIo(int timeoutMs) override;

    void simulateExternalDisconnect();
    void setResponseDelay(int iMs);
    void setDelayFollowsTimeout(bool followTimeout);
    void setResponses(QList<QByteArray> responseList);
    void appendResponses(QList<QByteArray> responseList);

    virtual SourceInterfaceTypes type() override { return SOURCE_INTERFACE_DEMO; }
    virtual bool isOpen() override { return m_bOpen; }

protected:
    explicit cSourceInterfaceDemo(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;
private slots:
    void onResponseDelayTimer();
private:
    void sendResponse(bool error);

    bool m_bOpen = false;
    int m_responseDelayMs = 0;
    int m_responseDelayMsTimeoutSimul = sourceDefaultTimeout/2;
    bool m_delayFollowsTimeout = false;
    QTimer m_responseDelayTimer;
    QByteArray* m_pDataReceive = nullptr;
    QList<QByteArray> m_responseList;
};


// ------------------------- ZERA serial interface --------------------------
class cSourceInterfaceZeraSerialPrivate;
class cSourceInterfaceZeraSerial : public cSourceInterfaceBase
{
    Q_OBJECT
public:
    virtual ~cSourceInterfaceZeraSerial();

    virtual SourceInterfaceTypes type() override { return SOURCE_INTERFACE_ASYNCSERIAL; }
    virtual bool open(QString strDeviceInfo) override; // e.g "/dev/ttyUSB0"
    virtual void close() override;
    virtual void setReadTimeoutNextIo(int timeoutMs) override;
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive) override;

    // wrappers - see https://github.com/ZeraGmbH/qtiohelper / QT += serialportasyncblock
    void setReadTimeoutNextIo(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal = 0);
    void setBlockEndCriteriaNextIo(int iBlockLenReceive = 0, QByteArray endBlock = QByteArray());

    virtual bool isOpen() override;

protected:
    explicit cSourceInterfaceZeraSerial(QObject *parent = nullptr);
    friend class cSourceInterfaceFactory;
private slots:
    void onIoFinished();
    void onDeviceFileGone(QString);
private:
    cSourceInterfaceZeraSerialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(cSourceInterfaceZeraSerial)
};

#endif // SOURCEINTERFACEBASE_H

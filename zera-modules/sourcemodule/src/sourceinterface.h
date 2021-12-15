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

class SourceInterfaceBase;
typedef QSharedPointer<SourceInterfaceBase> tSourceInterfaceShPtr;

// create interfaces through SourceInterfaceFactory only
class SourceInterfaceFactory {
public:
    static tSourceInterfaceShPtr createSourceInterface(SourceInterfaceTypes type, QObject *parent = nullptr);
};


// ------------------------- base interface --------------------------
class SourceInterfaceBase : public QObject
{
    Q_OBJECT
public:
    virtual ~SourceInterfaceBase();

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
    explicit SourceInterfaceBase(QObject *parent = nullptr);
    friend class SourceInterfaceFactory;

    QString m_strDeviceInfo;
    cSourceIdGenerator m_IDGenerator;
    int m_currentIoID = 0;
};


// ------------------------- demo interface --------------------------
class SourceInterfaceDemo : public SourceInterfaceBase
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
    explicit SourceInterfaceDemo(QObject *parent = nullptr);
    friend class SourceInterfaceFactory;
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
class SourceInterfaceZeraSerialPrivate;
class SourceInterfaceZeraSerial : public SourceInterfaceBase
{
    Q_OBJECT
public:
    virtual ~SourceInterfaceZeraSerial();

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
    explicit SourceInterfaceZeraSerial(QObject *parent = nullptr);
    friend class SourceInterfaceFactory;
private slots:
    void onIoFinished();
    void onDeviceFileGone(QString);
private:
    SourceInterfaceZeraSerialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(SourceInterfaceZeraSerial)
};

#endif // SOURCEINTERFACEBASE_H

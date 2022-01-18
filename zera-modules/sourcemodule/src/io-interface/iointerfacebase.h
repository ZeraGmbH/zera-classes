#ifndef IOINTERFACEBASE_H
#define IOINTERFACEBASE_H

#include "iointerfacefactory.h"
#include "io-ids/ioidgenerator.h"
#include "io-ids/ioidkeeper.h"

#include <QObject>

static constexpr int sourceDefaultTimeout = 1500;

class IoInterfaceBase : public QObject
{
    Q_OBJECT
public:
    virtual ~IoInterfaceBase();

    virtual bool open(QString) { return false; }
    virtual void close() {}
    virtual void setReadTimeoutNextIo(int) {};
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive);
    virtual void simulateExternalDisconnect() {}

    virtual bool isOpen() { return false; }
    bool isDemo() { return type() == SOURCE_INTERFACE_DEMO; }
    QString getDeviceInfo();
    virtual IoInterfaceTypes type() { return SOURCE_INTERFACE_BASE; }

signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool interfaceError); // users connect this signal
    void sigIoFinishedToQueue(int ioID, bool interfaceError); // sub classes emit this to ensure queue

protected:
    explicit IoInterfaceBase(QObject *parent = nullptr);
    friend class IoInterfaceFactory;

    QString m_strDeviceInfo;
    IoIdGenerator m_IDGenerator;
    IoIdKeeper m_currIoId;
};


#endif // IOINTERFACEBASE_H

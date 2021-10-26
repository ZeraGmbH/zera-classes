#ifndef IOINTERFACE_H
#define IOINTERFACE_H

#include <QObject>

class cIOTransaktionInfo;
class cIOTransaktionResult;

class cIOInterface : public QObject
{
    Q_OBJECT
public:
    enum IOType {
        IO_DEMO,
        IO_ASYNC_SERIAL,
    };

    explicit cIOInterface(enum IOType ioType, QString deviceFilename, QObject *parent = nullptr);
    virtual ~cIOInterface();

    // getter
    enum IOType type();
    QString deviceFileName();

    // requests
    void requestExternalDisconnect();
    void startTransaction(cIOTransaktionInfo& transaction);

signals:
    void sigTransactionFinshed(cIOTransaktionResult* result);
    void sigDisconnected(cIOInterface* pIOInterface); // either disconnect or external request

private:
    enum IOType m_ioType;
    QString m_deviceFilename;
};

#endif // IOINTERFACE_H

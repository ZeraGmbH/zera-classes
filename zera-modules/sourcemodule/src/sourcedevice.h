#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>

class cIOInterface;

namespace SOURCEMODULE
{
class cSourceDevice : public QObject
{
    Q_OBJECT
public:
    enum SourceType { // TODO replace by protocol
        SOURCE_DEMO,
        SOURCE_MT3000,
        SOURCE_MT400_20,
    };
    explicit cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent = nullptr);
    virtual ~cSourceDevice();

    // requests
    void close();

    // getter
    cIOInterface* ioInterface();
    QString name();
    QString deviceInfo();

    // setter
    void setName(QString name);

signals:
    void sigClosed(cSourceDevice* sourceDevice);

private slots:
    void onInterfaceClosed(cIOInterface *ioInterface);

private:
    // we own interface
    cIOInterface* m_IOInterface;

    SourceType m_type;
    QString m_deviceName;
};

}
#endif // CSOURCEDEVICE_H

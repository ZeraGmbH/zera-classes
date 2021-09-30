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
        SOURCE_DEMO = 0,
        SOURCE_MT3000,
        SOURCE_MT400_20,

        SOURCE_DEMO_FG_4PHASE,

        SOURCE_TYPE_COUNT
    };
    explicit cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent = nullptr);
    virtual ~cSourceDevice();

    // requests
    void close();

    // getter
    cIOInterface* ioInterface();
    QString deviceInfo();

signals:
    void sigClosed(cSourceDevice* sourceDevice);

private slots:
    void onInterfaceClosed(cIOInterface *ioInterface);

private:
    // we own interface
    cIOInterface* m_IOInterface;

    SourceType m_type;
    SourceType m_demoType;
};

}
#endif // CSOURCEDEVICE_H

#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <zera-json-params-structure.h>

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
        SOURCE_MT_SINGLE_PHASE,

        SOURCE_DEMO_FG_4PHASE,

        SOURCE_TYPE_COUNT
    };
    explicit cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent = nullptr);
    virtual ~cSourceDevice();
    cZeraJsonParamsStructure* paramsStructure();

    // requests
    void close();

    // getter
    cIOInterface* ioInterface();
    QJsonObject deviceParamInfo();
    QJsonObject deviceParamState();

signals:
    void sigClosed(cSourceDevice* sourceDevice);

private slots:
    void onInterfaceClosed(cIOInterface *ioInterface);

private:
    QString deviceFileName();

    cIOInterface* m_IOInterface; // WE own the interface
    cZeraJsonParamsStructure m_ZeraJsonParamsStructure;
    QJsonObject m_paramState;

    SourceType m_type;
    SourceType m_demoType;
};

}
#endif // CSOURCEDEVICE_H

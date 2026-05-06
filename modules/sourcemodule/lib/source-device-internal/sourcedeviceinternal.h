#ifndef SOURCEDEVICEINTERNAL_H
#define SOURCEDEVICEINTERNAL_H

#include "sourcedevicetemplate.h"
#include "sourcestates.h"
#include <pcbinterface.h>

class SourceDeviceInternal : public SourceDeviceTemplate
{
    Q_OBJECT
public:
    explicit SourceDeviceInternal(AbstractServerInterfacePtr serverInterface,
                                  const QJsonObject &sourceCapabilities);

    void switchLoad(const QJsonObject &params) override;
    void close(QUuid uuid) override;
private slots:
    void onSourceSwitchFinished(bool ok);
private:
    SourceStates m_currState = SourceStates::UNDEFINED;
};

#endif // SOURCEDEVICEINTERNAL_H

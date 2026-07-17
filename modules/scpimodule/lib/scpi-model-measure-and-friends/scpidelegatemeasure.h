#ifndef SCPIDELEGATEMEASURE_H
#define SCPIDELEGATEMEASURE_H

#include "scpidelegatetemplate.h"
#include "scpimodeldefinitions.h"
#include "scpiclientexecutormeasure.h"
#include <QJsonObject>
#include <QUuid>
#include <memory>

namespace SCPIMODULE {

class ScpiDelegateMeasure : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    struct Params {
        const QString cmdParent;
        const QString cmd;
        const quint8 scpiQueryCmdFlags = 0;
        const ScpiModelTypes modelType = MEASURE;
        const QJsonObject veinComponentInfo;
    };
    explicit ScpiDelegateMeasure(const Params &params);
    void addComponentId(const VeinComponentId &componentId);
    void removeClient(cSCPIClient *client);

    void executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId) override;

private:
    ScpiClientExecutorMeasurePtr getExecutor(cSCPIClient *client);

    const Params m_params;
    QList<VeinComponentId> m_veinComponents;
    QHash<QUuid, ScpiClientExecutorMeasurePtr> m_clientExecutors;
};

typedef std::shared_ptr<ScpiDelegateMeasure> ScpiDelegateMeasurePtr;

}
#endif // SCPIDELEGATEMEASURE_H

#ifndef TESTACTUALVALUEGENERATORRMS_H
#define TESTACTUALVALUEGENERATORRMS_H

#include "abstractactualvaluegenerator.h"

class TestActualValueGeneratorRms : public AbstractActualValueGenerator
{
    Q_OBJECT
public:
    void onNewActualValues(QVector<float> *actValues) override;
    void start() override;
    void stop() override;
private:
    QVector<float> m_actValues;
    bool m_started = false;
};

#endif // TESTACTUALVALUEGENERATORRMS_H

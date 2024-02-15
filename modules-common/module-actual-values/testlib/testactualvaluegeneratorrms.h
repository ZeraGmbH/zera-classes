#ifndef TESTACTUALVALUEGENERATORRMS_H
#define TESTACTUALVALUEGENERATORRMS_H

#include "abstractactualvaluegenerator.h"

class TestActualValueGeneratorRms : public AbstractActualValueGenerator
{
    Q_OBJECT
public:
    TestActualValueGeneratorRms(QStringList valueChannelList);

    void onNewActualValues(QVector<float> *actValues) override;
    void start() override;
    void stop() override;

    QStringList getValueChannelList();
private:
    QStringList m_valueChannelList;
    QVector<float> m_actValues;
    bool m_started = false;
};

typedef std::shared_ptr<TestActualValueGeneratorRms> TestActualValueGeneratorRmsPtr;

#endif // TESTACTUALVALUEGENERATORRMS_H

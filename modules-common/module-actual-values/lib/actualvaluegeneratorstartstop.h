#ifndef ACTUALVALUEGENERATORSTARTSTOP_H
#define ACTUALVALUEGENERATORSTARTSTOP_H

#include "abstractactualvaluegenerator.h"

class ActualValueGeneratorStartStop : public AbstractActualValueGenerator
{
    Q_OBJECT
public:
    void onNewActualValues(QVector<float> *values) override;
    void start() override;
    void stop() override;
private:
    bool m_started = false;
};

#endif // ACTUALVALUEGENERATORSTARTSTOP_H

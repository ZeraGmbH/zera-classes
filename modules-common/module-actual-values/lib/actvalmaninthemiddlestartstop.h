#ifndef ACTVALMANINTHEMIDDLESTARTSTOP_H
#define ACTVALMANINTHEMIDDLESTARTSTOP_H

#include "abstractactvalmaninthemiddle.h"

class ActValManInTheMiddleStartStop : public AbstractActValManInTheMiddle
{
    Q_OBJECT
public:
    void onNewActualValues(QVector<float> *values) override;
    void start() override;
    void stop() override;
private:
    bool m_started = false;
};

#endif // ACTVALMANINTHEMIDDLESTARTSTOP_H

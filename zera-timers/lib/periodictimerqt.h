#ifndef PERIODICTIMERQT_H
#define PERIODICTIMERQT_H

#include "zeratimertemplate.h"
#include <QTimer>
#include <memory>

class PeriodicTimerQt : public ZeraTimerTemplate
{
    Q_OBJECT
public:
    explicit PeriodicTimerQt(int expireTimeMs);
    void setHighAccuracy(bool on);
    void start() override;
    void stop() override;
private:
    std::unique_ptr<QTimer> m_qtTimer;
    bool m_highAccuracy = false;
};

#endif // PERIODICTIMERQT_H

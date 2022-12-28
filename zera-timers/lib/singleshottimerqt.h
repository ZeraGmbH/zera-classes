#ifndef SINGLESHOTTIMERQT_H
#define SINGLESHOTTIMERQT_H

#include "zeratimertemplate.h"
#include <QTimer>
#include <memory>

class SingleShotTimerQt : public ZeraTimerTemplate
{
    Q_OBJECT
public:
    explicit SingleShotTimerQt(int expireTimeMs);
    void setHighAccuracy(bool on);
    void start() override;
    void stop() override;
private:
    std::unique_ptr<QTimer> m_qtTimer;
    bool m_highAccuracy = false;
};

#endif // SINGLESHOTTIMERQT_H

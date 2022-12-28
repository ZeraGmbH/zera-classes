#include "singleshottimerqt.h"

SingleShotTimerQt::SingleShotTimerQt(int expireTimeMs) :
    ZeraTimerTemplate(expireTimeMs)
{
}

void SingleShotTimerQt::setHighAccuracy(bool on)
{
    m_highAccuracy = on;
}

void SingleShotTimerQt::start()
{
    m_qtTimer = std::make_unique<QTimer>();
    m_qtTimer->setSingleShot(true);
    m_qtTimer->setTimerType(m_highAccuracy ? Qt::PreciseTimer: Qt::CoarseTimer);
    connect(m_qtTimer.get(), &QTimer::timeout,
            this, &SingleShotTimerQt::sigExpired);
    m_qtTimer->start(m_expireTimeMs);
}

void SingleShotTimerQt::stop()
{
    m_qtTimer = nullptr;
}

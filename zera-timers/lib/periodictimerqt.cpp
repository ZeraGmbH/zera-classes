#include "periodictimerqt.h"

PeriodicTimerQt::PeriodicTimerQt(int expireTimeMs) :
    ZeraTimerTemplate(expireTimeMs)
{
}

void PeriodicTimerQt::setHighAccuracy(bool on)
{
    m_highAccuracy = on;
}

void PeriodicTimerQt::start()
{
    m_qtTimer = std::make_unique<QTimer>();
    m_qtTimer->setSingleShot(false);
    m_qtTimer->setTimerType(m_highAccuracy ? Qt::PreciseTimer: Qt::CoarseTimer);
    connect(m_qtTimer.get(), &QTimer::timeout,
            this, &PeriodicTimerQt::sigExpired);
    m_qtTimer->start(m_expireTimeMs);
}

void PeriodicTimerQt::stop()
{
    m_qtTimer = nullptr;
}

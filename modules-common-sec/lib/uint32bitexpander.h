#ifndef UINT32BITEXPANDER_H
#define UINT32BITEXPANDER_H

#include <timerelapsedtemplateqt.h>
#include <timerfactoryqt.h>

// Assumptions - see tests
// * actual is countining upwards mononotonic
// * final is close to actual
class UInt32BitExpander
{
public:
    // * We had to learn the hard way that sec and friends sporadically come
    //   along with timer values from previous measurement. They caused
    //   us detect a wraparound on next value belonging to the new measurement.
    // * Let's not call this a hack - it simply makes the code more robust and
    //   no 32 bit timer around runs with 2³² / 10s => 429 496 729.6Hz ~ 400Mhz
    //   (remember nominal frequency is 0.2MHz)
    static constexpr quint32 SafeDelayBeforFirstWraparound = 10*1000;

    void resetForNextPeriod();

    void setActual32(quint32 value);
    quint64 getActualExpanded64() const;

    void setFinal32(quint32 value);
    quint64 getFinalExpanded64() const;

    static double uint64ToDbl(const quint64 &value);

private:
    void adjustFinalOffsetToActual();

    TimerElapsedTemplateQtPtr m_elapsedTimerForStartPeriod = TimerFactoryQt::createElapsed();

    quint32 m_actualLower = 0;
    quint32 m_actualUpper = 0;

    quint32 m_finalLower = 0;
    qint32 m_signedOffsetFinalToActual = 0;
};

#endif // UINT32BITEXPANDER_H

#ifndef DEMOACTVALMANINTHEMIDDLERMS_H
#define DEMOACTVALMANINTHEMIDDLERMS_H

#include "abstractactvalmaninthemiddle.h"
#include <timertemplateqt.h>
#include <QStringList>

class DemoActValManInTheMiddleRms : public AbstractActValManInTheMiddle
{
    Q_OBJECT
public:
    DemoActValManInTheMiddleRms(QStringList valueChannelList);
    void onNewActualValues(QVector<float> *) override { };
    void start() override;
    void stop() override;

private slots:
    void onNewActValues();
private:
    QStringList m_valueChannelList;
    TimerTemplateQtPtr m_periodicTimerActGen;
    QVector<float> m_demoValues;
};

#endif // DEMOACTVALMANINTHEMIDDLERMS_H

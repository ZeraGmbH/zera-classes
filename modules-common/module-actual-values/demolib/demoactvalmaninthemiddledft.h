#ifndef DEMOACTVALMANINTHEMIDDLEDFT_H
#define DEMOACTVALMANINTHEMIDDLEDFT_H

#include <abstractactvalmaninthemiddle.h>
#include <timertemplateqt.h>
#include <QStringList>

class DemoActValManInTheMiddleDft : public AbstractActValManInTheMiddle
{
    Q_OBJECT
public:
    DemoActValManInTheMiddleDft(QStringList valueChannelList);
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

#endif // DEMOACTVALMANINTHEMIDDLEDFT_H

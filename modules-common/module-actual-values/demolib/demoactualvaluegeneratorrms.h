#ifndef DEMOACTUALVALUEGENERATORRMS_H
#define DEMOACTUALVALUEGENERATORRMS_H

#include "abstractactualvaluegenerator.h"
#include <timertemplateqt.h>
#include <QStringList>

class DemoActualValueGeneratorRms : public AbstractActualValueGenerator
{
    Q_OBJECT
public:
    DemoActualValueGeneratorRms(QStringList valueChannelList);
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

#endif // DEMOACTUALVALUEGENERATORRMS_H

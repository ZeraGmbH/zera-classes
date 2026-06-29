#ifndef DOSAGEMODULEMEASPROGRAM_H
#define DOSAGEMODULEMEASPROGRAM_H

#include "basemeasworkprogram.h"
#include "dosagemoduleconfigdata.h"
#include <vfmoduleparameter.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <vs_storagecomponent.h>
#include <timerperiodicqt.h>

namespace DOSAGEMODULE
{

struct dosagepoweranalyser : dosagesystemconfiguration
{
    VeinStorage::AbstractComponentPtr m_component;
};

class cDosageModule;

class cDosageModuleMeasProgram : public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    explicit cDosageModuleMeasProgram(cDosageModule *module);
    void generateVeinInterface() override; // here we export our interface (entities)
    void setupMeasureProgram();

public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override;  // in interface are not updated when stop

private:
    cDosageModule *m_pModule = nullptr;

    VfModuleParameter* m_pPowerDetected = nullptr;

    QList<dosagepoweranalyser> m_PowerToAnalyseList;

    QFinalState m_activationDoneState;
    QFinalState m_deactivateDoneState;

    TimerTemplateQtPtr m_ActualizeTimer;
    static constexpr quint32 m_nActualizeTimer = 1000;

private slots:
    void activateDone();
    void deactivateMeasDone();
    void onActualize();
};

}
#endif // DOSAGEMODULEMEASPROGRAM_H

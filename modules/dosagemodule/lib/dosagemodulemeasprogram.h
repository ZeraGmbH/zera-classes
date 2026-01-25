#ifndef DOSAGEMODULEMEASPROGRAM_H
#define DOSAGEMODULEMEASPROGRAM_H

#include "dosagemoduleconfigdata.h"
#include "basemeasworkprogram.h"
//#include "vfmoduleactvalue.h"
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
    cDosageModuleMeasProgram(cDosageModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~cDosageModuleMeasProgram();
    void generateVeinInterface() override; // here we export our interface (entities)
    void setupMeasureProgram();

public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override;  // in interface are not updated when stop

private:
    cDosageModuleConfigData *getConfData();

    cDosageModule *m_pModule;

    VfModuleParameter* m_pPowerDetected;

    QList<dosagepoweranalyser> m_PowerToAnalyseList;

    // statemachine for activating gets the following states
    QFinalState m_activationDoneState;

    // statemachine for deactivating
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

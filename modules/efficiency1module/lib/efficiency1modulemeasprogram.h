#ifndef EFFICIENCY1MODULEMEASPROGRAM_H
#define EFFICIENCY1MODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "efficiency1moduleconfigdata.h"
#include "basemeasworkprogram.h"


class cBaseModule;
class VfModuleActvalue;
class VfModuleMetaData;
class VfModuleComponent;

namespace EFFICIENCY1MODULE
{

class cEfficiency1ModuleConfigData;
class cEfficiency1MeasDelegate;
class cEfficiency1Module;


class cEfficiency1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    cEfficiency1ModuleMeasProgram(cEfficiency1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cEfficiency1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cEfficiency1ModuleConfigData* getConfData();

    cEfficiency1Module* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pEFFCountInfo; // the number of values we produce
    VfModuleComponent* m_pMeasureSignal;

    QList<cEfficiency1MeasDelegate*> m_Efficiency1MeasDelegateList;

    // statemachine for activating gets the following states
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivateState;
    QFinalState m_deactivateDoneState;

private slots:
    void searchActualValues();
    void activateDone();

    void deactivateMeas();
    void deactivateMeasDone();

    void setMeasureSignal(int signal);
};

}
#endif // EFFICIENCY1MODULEMEASPROGRAM_H

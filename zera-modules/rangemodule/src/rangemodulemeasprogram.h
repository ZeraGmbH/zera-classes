#ifndef RANGEMODULEMEASPROGRAM_H
#define RANGEMODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "basemeasprogram.h"


enum rangemoduleCmds
{
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    dataaquistion
};

class VeinPeer;
class VeinEntity;
class cBaseModule;
class cDspMeasData;
class cDspIFace;
class QStateMachine;
class QState;
class cRangeModule;
class QFinalState;


class cRangeModuleMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT

public:
    cRangeModuleMeasProgram(cRangeModule* module, VeinPeer* peer, Zera::Server::cDSPInterface* iface, cSocket* rmsocket, QStringList chnlist);
    virtual ~cRangeModuleMeasProgram();

public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

signals:
    void activationContinue(); // for internal loop control

protected:
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    virtual void setDspVarList(); // dsp related stuff
    virtual void deleteDspVarList();
    virtual void setDspCmdList();
    virtual void deleteDspCmdList();

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr,QVariant answer);

private:
    cRangeModule* m_pModule; // the module we live in
    QList<VeinEntity*> m_EntityList;
    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for loading a dsp program
    QStateMachine m_loadDSPMachine;
    QState m_var2DSPState, m_cmd2DSPState, m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for unloading a dsp program;
    QStateMachine m_unloadDSPMachine;
    QState m_deactivateDSPState;
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

private slots:
    void setInterfaceActualValues(QVector<double>* actualValues);

    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSP();
    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

};

#endif // RANGEMODULEMEASPROGRAM_H

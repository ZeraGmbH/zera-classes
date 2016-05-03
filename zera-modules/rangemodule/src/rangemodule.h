#ifndef RANGEMODULE_H
#define RANGEMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include <basemeasmodule.h>


namespace Zera
{
    namespace Server
    {
        class cDSPInterface;
    }
    namespace Proxy
    {
        class cProxyClient;
    }
}



class cVeinModuleMetaData;

namespace RANGEMODULE
{

class cRangeMeasChannel;
class cRangeModuleConfiguration;
class cRangeModuleMeasProgram;
class cAdjustManagement;
class cRangeObsermatic;
class cRangeModuleObservation;


#define BaseModuleName "RangeModule"
#define BaseSCPIModuleName "RNG"

class cRangeModule : public cBaseMeasModule
{
Q_OBJECT

public:
    cRangeModule(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual ~cRangeModule();
    virtual QByteArray getConfiguration() const;
    virtual cRangeMeasChannel* getMeasChannel(QString name); // also used for callback

protected:
    cRangeModuleObservation *m_pRangeModuleObservation;
    cRangeModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    cAdjustManagement * m_pAdjustment; // our justifying and normation program
    cRangeObsermatic *m_pRangeObsermatic; // our range handling
    QList<cRangeMeasChannel*> m_rangeMeasChannelList; // our meas channels
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void unsetModule(); // in case of reconfiguration we must unset module first
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

protected slots:
    virtual void activationStart();
    virtual void activationExec();
    virtual void activationDone();
    virtual void activationFinished();

    virtual void deactivationStart();
    virtual void deactivationExec();
    virtual void deactivationDone();
    virtual void deactivationFinished();

private:
    cVeinModuleMetaData* m_pChannelCountInfo;
    cVeinModuleMetaData* m_pGroupCountInfo;
    qint32 m_nActivationIt;

private slots:


    void rangeModuleReconfigure();

};

}

#endif // RANGEMODULE_H

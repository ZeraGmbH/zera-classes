#ifndef SOURCEMODULEPROGRAM_H
#define SOURCEMODULEPROGRAM_H

#include <QList>
#include <QHash>
#include <QTimer>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "basemeasprogram.h"

namespace Zera
{
namespace Proxy
{
    class cProxyClient;
}
}


class cVeinModuleParameter;
class cVeinModuleActvalue;
class cStringValidator;

class cDspMeasData;
class cDspIFace;
class QStateMachine;
class QState;
class QFinalState;


namespace SOURCEMODULE
{

enum sourcemoduleCmds
{
};

class cSourceModule;
class cSourceModuleConfigData;

class cSourceModuleProgram: public cBaseMeasProgram
{
    Q_OBJECT

public:
    cSourceModuleProgram(cSourceModule* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cSourceModuleProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop


protected slots:
    //virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cSourceModuleConfigData* getConfData();

    cSourceModule* m_pModule; // the module we live in

    cVeinModuleParameter* m_pMaxCountSources;

    // methods
    void setInterfaceComponents();
    void setValidators();

private slots:
    // vein change handlers

};
}

#endif // SOURCEMODULEPROGRAM_H

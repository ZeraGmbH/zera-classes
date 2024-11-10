#ifndef BASEDSPMEASPROGRAM_H
#define BASEDSPMEASPROGRAM_H

#include "basemeasprogram.h"
#include "abstractfactoryserviceinterfaces.h"

class cBaseDspMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT
public:
    cBaseDspMeasProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration, QString moduleName);
    virtual ~cBaseDspMeasProgram() { }
protected:
    // the module creates a central dsp interface and forwards this
    // the reason behind this is, that dsp server generates a new "environment" for each
    // new client. but we want to access the same environment's value from all over the module.
    DspInterfacePtr m_dspInterface; // our interface to dsp
    Zera::ProxyClientPtr m_dspClient;
    quint32 m_nDspMemUsed;

};

#endif // BASEDSPMEASPROGRAM_H

#ifndef BASEDSPMEASPROGRAM_H
#define BASEDSPMEASPROGRAM_H

#include "basemeasprogram.h"
#include <dspinterface.h>

class cBaseDspMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT
public:
    cBaseDspMeasProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration, QString moduleName);
    virtual ~cBaseDspMeasProgram() { }
signals:
    void dataAquisitionContinue();
protected:
    // the module creates a central dsp interface and forwards this
    // the reason behind this is, that dsp server generates a new "environment" for each
    // new client. but we want to access the same environment's value from all over the module.
    Zera::DspInterfacePtr m_dspInterface; // our interface to dsp
    Zera::ProxyClientPtr m_dspClient;
};

#endif // BASEDSPMEASPROGRAM_H

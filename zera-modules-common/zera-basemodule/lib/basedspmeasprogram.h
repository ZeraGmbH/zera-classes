#ifndef BASEDSPMEASPROGRAM_H
#define BASEDSPMEASPROGRAM_H

#include "basemeasprogram.h"
#include <dspinterface.h>

class cBaseDspMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT
public:
    cBaseDspMeasProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cBaseDspMeasProgram() { }
protected:
    virtual void deleteDspVarList() = 0;
    virtual void setDspCmdList() = 0;
    virtual void deleteDspCmdList() = 0;
    // the module creates a central dsp interface and forwards this
    // the reason behind this is, that dsp server generates a new "environment" for each
    // new client. but we want to access the same environment's value from all over the module.
    Zera::cDSPInterface* m_pDSPInterFace; // our interface to dsp
    Zera::ProxyClient *m_pDspClient;
    quint32 m_nDspMemUsed;

};

#endif // BASEDSPMEASPROGRAM_H

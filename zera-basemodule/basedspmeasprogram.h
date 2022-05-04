#ifndef BASEDSPMEASPROGRAM_H
#define BASEDSPMEASPROGRAM_H

#include "basemeasprogram.h"
#include <service-interfaces/dspinterface.h>

class cBaseDspMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT
public:
    cBaseDspMeasProgram(Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
protected:
    virtual void setDspVarList() = 0; // dsp related stuff
    virtual void deleteDspVarList() = 0;
    virtual void setDspCmdList() = 0;
    virtual void deleteDspCmdList() = 0;
    // the module creates a central dsp interface and forwards this
    // the reason behind this is, that dsp server generates a new "environment" for each
    // new client. but we want to access the same environment's value from all over the module.
    Zera::Server::cDSPInterface* m_pDSPInterFace; // our interface to dsp
    Zera::Proxy::cProxyClient *m_pDspClient;
    quint32 m_nDspMemUsed;

};

#endif // BASEDSPMEASPROGRAM_H

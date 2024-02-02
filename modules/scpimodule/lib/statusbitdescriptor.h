#ifndef STATUSBITDESCRIPTOR
#define STATUSBITDESCRIPTOR

namespace SCPIMODULE
{

// used for configuration export


struct cStatusBitDescriptor
{
    quint8 m_nBitNr;
    QString m_sSCPIModuleName; //
    QString m_sComponentName;
};
}

#endif // STATUSBITDESCRIPTOR


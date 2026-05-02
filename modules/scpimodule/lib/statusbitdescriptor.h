#ifndef STATUSBITDESCRIPTOR
#define STATUSBITDESCRIPTOR

#include <QString>

namespace SCPIMODULE
{
struct cStatusBitDescriptor
{
    quint8 m_nBitNr;
    QString m_sSCPIModuleName;
    QString m_sComponentName;
};
}

#endif // STATUSBITDESCRIPTOR


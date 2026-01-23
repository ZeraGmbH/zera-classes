#ifndef POWER3MEASDELEGATE
#define POWER3MEASDELEGATE

#include <vs_abstractcomponent.h>
#include <QObject>

class VfModuleComponent;

namespace POWER3MODULE
{

class cPower3MeasDelegate : public QObject
{
    Q_OBJECT
public:
    cPower3MeasDelegate(VeinStorage::AbstractComponentPtr fftU,
                        VeinStorage::AbstractComponentPtr fftI,
                        VfModuleComponent *pactvalue,
                        VfModuleComponent *qactvalue,
                        VfModuleComponent *sactvalue);
public:
    void computeOutput();
private:
    VeinStorage::AbstractComponentPtr m_fftU;
    VeinStorage::AbstractComponentPtr m_fftI;
    VfModuleComponent *m_ppActValue;
    VfModuleComponent *m_pqActValue;
    VfModuleComponent *m_psActValue;
};

}

#endif // POWER3MEASDELEGATE


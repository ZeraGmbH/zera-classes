#ifndef CSOURCEVEININTERFACE_H
#define CSOURCEVEININTERFACE_H

#include <QObject>
class cVeinModuleActvalue;
class cVeinModuleParameter;
class cJsonParamValidator;

namespace SOURCEMODULE
{
class cSourceDevice;

class cSourceVeinInterface : public QObject
{
    Q_OBJECT
public:
    explicit cSourceVeinInterface(QObject *parent = nullptr);

    // getters
    cVeinModuleActvalue *veinDeviceInfo();
    cVeinModuleActvalue *veinDeviceState();
    // vein setter+validator
    cVeinModuleParameter *veinDeviceParameter();
    cJsonParamValidator *veinDeviceParameterValidator();

    // setters
    // vein getters
    void setVeinDeviceInfo(cVeinModuleActvalue *veinDeviceInfo);
    void setVeinDeviceState(cVeinModuleActvalue* veinDeviceState);
    // vein setter+validator
    void setVeinDeviceParameter(cVeinModuleParameter* veinDeviceParameter);
    void setVeinDeviceParameterValidator(cJsonParamValidator* veinDeviceParameterValidator);
signals:

private:
    cVeinModuleActvalue* m_veinDeviceInfo = nullptr;
    cVeinModuleActvalue* m_veinDeviceState = nullptr;
    cVeinModuleParameter* m_veinDeviceParameter = nullptr;
    cJsonParamValidator* m_veinDeviceParameterValidator = nullptr;
};

} // namespace SOURCEMODULE

#endif // CSOURCEVEININTERFACE_H

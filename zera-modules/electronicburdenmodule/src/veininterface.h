#ifndef VEININTERFACE_H
#define VEININTERFACE_H

#include <QObject>

class VfModuleActvalue;
class VfModuleParameter;
class cJsonParamValidator;

class VeinInterface : public QObject
{
    Q_OBJECT
public:
    explicit VeinInterface(QObject *parent = nullptr);
    // getters
    VfModuleActvalue *getVeinDeviceInfoComponent();
    VfModuleActvalue *getVeinDeviceStateComponent();
    VfModuleParameter *getVeinDeviceParameterComponent();
    cJsonParamValidator *getVeinDeviceParameterValidator();

    // setters
    void setVeinDeviceInfoComponent(VfModuleActvalue *veinDeviceInfo);
    void setVeinDeviceStateComponent(VfModuleActvalue* veinDeviceState);
    void setVeinDeviceParameterComponent(VfModuleParameter* veinDeviceParameter);
    void setVeinDeviceParameterValidator(cJsonParamValidator* veinDeviceParameterValidator);
signals:

private:
    VfModuleActvalue* m_veinDeviceInfo = nullptr;
    VfModuleActvalue* m_veinDeviceState = nullptr;
    VfModuleParameter* m_veinDeviceParameter = nullptr;
    cJsonParamValidator* m_veinDeviceParameterValidator = nullptr;

};

#endif // VEININTERFACE_H

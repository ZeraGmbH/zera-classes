#ifndef VEININTERFACE_H
#define VEININTERFACE_H

#include <QObject>

class cVeinModuleActvalue;
class cVeinModuleParameter;
class cJsonParamValidator;

class VeinInterface : public QObject
{
    Q_OBJECT
public:
    explicit VeinInterface(QObject *parent = nullptr);
    // getters
    cVeinModuleActvalue *getVeinDeviceInfo();
    cVeinModuleActvalue *getVeinDeviceState();
    cVeinModuleParameter *getVeinDeviceParameter();
    cJsonParamValidator *getVeinDeviceParameterValidator();

    // setters
    void setVeinDeviceInfo(cVeinModuleActvalue *getVeinDeviceInfo);
    void setVeinDeviceState(cVeinModuleActvalue* getVeinDeviceState);
    void setVeinDeviceParameter(cVeinModuleParameter* getVeinDeviceParameter);
    void setVeinDeviceParameterValidator(cJsonParamValidator* getVeinDeviceParameterValidator);
signals:

private:
    cVeinModuleActvalue* m_veinDeviceInfo = nullptr;
    cVeinModuleActvalue* m_veinDeviceState = nullptr;
    cVeinModuleParameter* m_veinDeviceParameter = nullptr;
    cJsonParamValidator* m_veinDeviceParameterValidator = nullptr;

};

#endif // VEININTERFACE_H

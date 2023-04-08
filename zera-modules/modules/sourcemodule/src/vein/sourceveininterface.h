#ifndef CSOURCEVEININTERFACE_H
#define CSOURCEVEININTERFACE_H

#include <QObject>
#include <QJsonObject>
#include <QVariant>

class VfModuleActvalue;
class VfModuleParameter;
class cJsonParamValidator;

class SourceVeinInterface : public QObject
{
    Q_OBJECT
public:
    explicit SourceVeinInterface();

    VfModuleActvalue *getVeinDeviceInfoComponent();
    VfModuleActvalue *getVeinDeviceStateComponent();
    VfModuleParameter *getVeinDeviceParameterComponent();
    cJsonParamValidator *getVeinDeviceParameterValidator();

    void setVeinDeviceInfoComponent(VfModuleActvalue *veinDeviceInfo);
    void setVeinDeviceStateComponent(VfModuleActvalue* veinDeviceState);
    void setVeinDeviceParameterComponent(VfModuleParameter* veinDeviceParameter);
    void setVeinDeviceParameterValidator(cJsonParamValidator* veinDeviceParameterValidator);
signals:
    void sigNewLoadParams(QJsonObject params);

private slots:
    void onNewVeinLoadParams(QVariant params);
private:
    VfModuleActvalue* m_veinDeviceInfo = nullptr;
    VfModuleActvalue* m_veinDeviceState = nullptr;
    VfModuleParameter* m_veinDeviceParameter = nullptr;
    cJsonParamValidator* m_veinDeviceParameterValidator = nullptr;
};

#endif // CSOURCEVEININTERFACE_H

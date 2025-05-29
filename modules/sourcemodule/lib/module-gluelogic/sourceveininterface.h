#ifndef CSOURCEVEININTERFACE_H
#define CSOURCEVEININTERFACE_H

#include <QObject>
#include <QJsonObject>
#include <QVariant>

class VfModuleComponent;
class VfModuleParameter;
class cJsonParamValidator;

class SourceVeinInterface : public QObject
{
    Q_OBJECT
public:
    explicit SourceVeinInterface();

    VfModuleComponent *getVeinDeviceInfoComponent();
    VfModuleComponent *getVeinDeviceStateComponent();
    VfModuleParameter *getVeinDeviceParameterComponent();
    cJsonParamValidator *getVeinDeviceParameterValidator();

    void setVeinDeviceInfoComponent(VfModuleComponent *veinDeviceInfo);
    void setVeinDeviceStateComponent(VfModuleComponent* veinDeviceState);
    void setVeinDeviceParameterComponent(VfModuleParameter* veinDeviceParameter);
    void setVeinDeviceParameterValidator(cJsonParamValidator* veinDeviceParameterValidator);
signals:
    void sigNewLoadParams(QJsonObject params);

private slots:
    void onNewVeinLoadParams(QVariant params);
private:
    VfModuleComponent* m_veinDeviceInfo = nullptr;
    VfModuleComponent* m_veinDeviceState = nullptr;
    VfModuleParameter* m_veinDeviceParameter = nullptr;
    cJsonParamValidator* m_veinDeviceParameterValidator = nullptr;
};

#endif // CSOURCEVEININTERFACE_H

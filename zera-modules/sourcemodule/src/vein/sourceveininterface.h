#ifndef CSOURCEVEININTERFACE_H
#define CSOURCEVEININTERFACE_H

#include <QObject>
#include <QJsonObject>
#include <QVariant>

class cVeinModuleActvalue;
class cVeinModuleParameter;
class cJsonParamValidator;

class SourceVeinInterface : public QObject
{
    Q_OBJECT
public:
    explicit SourceVeinInterface(QObject *parent = nullptr);

    cVeinModuleActvalue *getVeinDeviceInfoComponent();
    cVeinModuleActvalue *getVeinDeviceStateComponent();
    cVeinModuleParameter *getVeinDeviceParameterComponent();
    cJsonParamValidator *getVeinDeviceParameterValidator();

    void setVeinDeviceInfoComponent(cVeinModuleActvalue *veinDeviceInfo);
    void setVeinDeviceStateComponent(cVeinModuleActvalue* veinDeviceState);
    void setVeinDeviceParameterComponent(cVeinModuleParameter* veinDeviceParameter);
    void setVeinDeviceParameterValidator(cJsonParamValidator* veinDeviceParameterValidator);
signals:
    void sigNewLoadParams(QJsonObject params);

private slots:
    void onNewVeinLoadParams(QVariant params);
private:
    cVeinModuleActvalue* m_veinDeviceInfo = nullptr;
    cVeinModuleActvalue* m_veinDeviceState = nullptr;
    cVeinModuleParameter* m_veinDeviceParameter = nullptr;
    cJsonParamValidator* m_veinDeviceParameterValidator = nullptr;
};

#endif // CSOURCEVEININTERFACE_H

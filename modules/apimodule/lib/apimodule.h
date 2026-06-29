#ifndef APIMODULE_H
#define APIMODULE_H

#include "vfeventsytemmoduleparam.h"
#include "apimoduleconfiguration.h"
#include <basemodule.h>
#include <vfrpceventsystem.h>

namespace APIMODULE
{
class cApiModule : public BaseModule
{
    Q_OBJECT
public:
    static constexpr const char *BaseModuleName = "ApiModule";
    static constexpr const char *BaseSCPIModuleName = "API";

    explicit cApiModule(const ModuleFactoryParam &moduleParam);
    cApiModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    VfEventSytemModuleParam* getValidatorEventSystem();
    VfRpcEventSystem *getRpcEventSystem() const;

    void setTrustListPath(const QString &path);

private slots:
    void activationFinished() override;
protected:
    void setupModule() override;                             // after xml configuration we can setup and export our module
    void startMeas() override;                               // we make the measuring program start here
    void stopMeas() override;

    cApiModuleConfiguration m_configuration;
    std::shared_ptr<VfEventSytemModuleParam> m_spModuleValidator;
    std::shared_ptr<VfRpcEventSystem> m_spRpcEventSystem;
    QString m_persistencyBasePath;
};

}

#endif // APIMODULE_H

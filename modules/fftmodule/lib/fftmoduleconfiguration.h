#ifndef FFTMODULECONFIGURATION_H
#define FFTMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "fftmoduleconfigdata.h"

namespace FFTMODULE
{

class cFftModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cFftModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cFftModuleConfigData* getConfigData();

protected slots:
    virtual void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);

private:
    void setConfiguration(const QByteArray& xmlString);
    cFftModuleConfigData m_configData;
};

}

#endif // FFTMODULCONFIGURATION_H

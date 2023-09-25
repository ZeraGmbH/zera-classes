#ifndef LICENSESYSTEMMOCK_H
#define LICENSESYSTEMMOCK_H

#include <licensesysteminterface.h>

class LicenseSystemMock : public LicenseSystemInterface
{
    Q_OBJECT
public:
    explicit LicenseSystemMock();
    virtual bool isSystemLicensed(const QString &uniqueModuleName) override;
    virtual bool serialNumberIsInitialized() const  override;
    virtual void processEvent(QEvent *event) override;
};

#endif // LICENSESYSTEMMOCK_H

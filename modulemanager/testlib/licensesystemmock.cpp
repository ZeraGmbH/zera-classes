#include "licensesystemmock.h"

LicenseSystemMock::LicenseSystemMock()
{
}

bool LicenseSystemMock::isSystemLicensed(const QString &uniqueModuleName)
{
    Q_UNUSED(uniqueModuleName)
    return true;
}

bool LicenseSystemMock::serialNumberIsInitialized() const
{
    return true;
}

void LicenseSystemMock::processEvent(QEvent *event)
{
    Q_UNUSED(event)
}

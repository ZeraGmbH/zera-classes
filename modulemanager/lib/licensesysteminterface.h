#ifndef LICENSESYSTEMINTERFACE_H
#define LICENSESYSTEMINTERFACE_H

#include <ve_eventsystem.h>

class LicenseSystemInterface : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    LicenseSystemInterface(QObject *parent = nullptr) :
        VeinEvent::EventSystem(parent) {}
    virtual bool isSystemLicensed(const QString &uniqueModuleName) = 0;
    virtual bool serialNumberIsInitialized() const  = 0;
signals:
    void sigSerialNumberInitialized();
};

#endif // LICENSESYSTEMINTERFACE_H

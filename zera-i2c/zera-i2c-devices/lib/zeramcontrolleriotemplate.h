#ifndef ZERAMCONTROLLERIOTEMPLATE_H
#define ZERAMCONTROLLERIOTEMPLATE_H

#include "zera-i2c-devices_export.h"
#include <QString>
#include <memory>

class ZERA_I2C_DEVICES_EXPORT ZeraMControllerIoTemplate
{
public:
    enum atmelRM
    {
        cmddone,
        cmdfault,
        cmdexecfault
    };
    ZeraMControllerIoTemplate(QString devnode, quint8 adr, quint8 debuglevel);
    virtual atmelRM bootloaderStartProgram() = 0;
protected:
    QString m_sI2CDevNode;
    quint8 m_nI2CAdr;
    quint8 m_nDebugLevel;
};

typedef std::shared_ptr<ZeraMControllerIoTemplate> ZeraMcontrollerIoPtr;

#endif // ZERAMCONTROLLERIOTEMPLATE_H

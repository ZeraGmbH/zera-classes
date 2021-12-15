#include "sourcedevicebase.h"
#include "jsonstructureloader.h"
#include "sourcejsonapi.h"

namespace SOURCEMODULE
{

SourceDeviceBase::SourceDeviceBase(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version) :
    QObject(nullptr),
    m_ioInterface(interface)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, version);
    m_outInGenerator = new cSourceIoPacketGenerator(paramStructure);

    /*if(deviceName.isEmpty()) {
        SourceJsonStructApi structApi(paramStructure);
        deviceName = structApi.getDeviceName();
    }*/
}

SourceDeviceBase::~SourceDeviceBase()
{
    delete m_outInGenerator;
}

bool SourceDeviceBase::isDemo()
{
    return m_ioInterface->isDemo();
}

QString SourceDeviceBase::getInterfaceDeviceInfo()
{
    return m_ioInterface->getDeviceInfo();
}


}

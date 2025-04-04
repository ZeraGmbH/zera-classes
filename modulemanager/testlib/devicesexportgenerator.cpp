#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include <QProcess>
#include <QDir>
#include <timemachinefortest.h>

DevicesExportGenerator::DevicesExportGenerator(QString xmlDirPath) :
    m_xmlDirPath(xmlDirPath)
{
}

void DevicesExportGenerator::exportAll(const LxdmSessionChangeParam &lxdmParam,
                                       AbstractFactoryServiceInterfacesPtr serviceInterFaceFactory,
                                       const QByteArray scpiCmd)
{
    QDir().mkdir(m_xmlDirPath);

    SessionExportGenerator sessionExportGenerator(lxdmParam);
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device, serviceInterFaceFactory);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            sessionExportGenerator.generateDevIfaceXml(m_xmlDirPath);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
            if(!scpiCmd.isEmpty()) {
                TimeMachineObject::feedEventLoop();
                TimeMachineForTest::getInstance()->processTimers(2000);
                m_scpiIoDumps[session] = sessionExportGenerator.sendScpi(scpiCmd);
            }
        }
    }
}

VeinDumps DevicesExportGenerator::getVeinDumps()
{
    return m_veinDumps;
}

ScpiIoDumps DevicesExportGenerator::getScpiIoDumps()
{
    return m_scpiIoDumps;
}

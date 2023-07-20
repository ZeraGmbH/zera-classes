#include "scpitestclient.h"

namespace SCPIMODULE {

ScpiTestClient::ScpiTestClient(cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface) :
    cSCPIClient(module, configdata, iface)
{
}

void ScpiTestClient::sendScpiCmds(QString cmds)
{
    QTextStream stream(&cmds);
    QString line;
    while (stream.readLineInto(&line)) {
        if(!line.endsWith("\n"))
            line += "\n";
        m_sInputFifo.append(line);
    }
    testCmd();
}

void ScpiTestClient::receiveAnswer(QString answ)
{
    emit sigScpiAnswer(answ);
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

}

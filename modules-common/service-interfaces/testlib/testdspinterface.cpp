#include "testdspinterface.h"
#include <dspinterface_p.h>

TestDspInterface::TestDspInterface(QStringList valueNamesList) :
    m_valueNamesList(valueNamesList)
{
}

QJsonObject TestDspInterface::dumpMemGroupsWritten()
{
    QJsonObject ret;
    for(int i=0; i<m_memgroupsWritten.count(); ++i) {
        const TMemGroupWritten &entry = m_memgroupsWritten[i];

        QString cmdString = entry.cmdWriteString;
        const QStringList cmdEntries = cmdString.split(";", Qt::SkipEmptyParts);

        for(const QString &cmdEntry : cmdEntries) {
            const QStringList entryParts = cmdEntry.split(",");
            QString varName = entryParts[0];
            QString groupVarName = entry.memGroupName + ":" + varName;

            QString transactionNum = QString("0000%1").arg(entry.transcationCount).right(4);
            QString key = QString("WriteNo: %1 / Handle:Variable: %2").arg(transactionNum, groupVarName);

            QString varValue = entryParts[1];
            ret.insert(key, varValue.toDouble());
        }
    }
    return ret;
}


quint32 TestDspInterface::dspMemoryWrite(DspMemoryGroups *memgroup)
{
    DspMemoryGroups *currentMemGroup = d_ptr->findMemHandle(memgroup->getName());
    bool nonEmptyMemgroupFound = false;
    if(currentMemGroup) {
        TMemGroupWritten toWrite = { m_transactionCount,
                                     currentMemGroup->getName(),
                                     currentMemGroup->getInterfaceWriteCommand() };
        m_memgroupsWritten.append(toWrite);
    }
    m_transactionCount++;

    emit sigDspMemoryWrite(memgroup->getName(), memgroup->getData());
    return sendCmdResponse("");
}

QStringList TestDspInterface::getValueList()
{
    return m_valueNamesList;
}

QJsonObject TestDspInterface::dumpAll(bool dumpVarWrite)
{
    QJsonObject dump;
    QJsonObject memGroups = dumpMemoryGroups();
    memGroups = dumpVarList(memGroups);
    dump.insert("1-MemGroups", memGroups);
    dump.insert("2-CmdList", QJsonArray::fromStringList(dumpCycListItem()));
    if(dumpVarWrite)
        dump.insert("3-VarsWritten", dumpMemGroupsWritten());
    return dump;
}

QJsonObject TestDspInterface::dumpMemoryGroups()
{
    const QList<DspMemoryGroups*> dspMemoryDataList = d_ptr->getMemoryDataList();
    QJsonObject dumpMemGroup;
    for(DspMemoryGroups* memData : dspMemoryDataList) {
        QJsonObject entry;
        entry.insert("Size", int(memData->getSize()));
        entry.insert("UserMemSize", int(memData->getUserMemSize()));
        dumpMemGroup.insert(memData->getName(), entry);
    }
    return dumpMemGroup;
}

QJsonObject TestDspInterface::dumpVarList(QJsonObject inData)
{
    const QStringList varList = d_ptr->varList2String().split(";", Qt::SkipEmptyParts);
    QMap<QString, QJsonArray> memGroupVariables;
    for(const QString &var : varList) {
        // see: DspMemoryGroups::getVarListLong
        // ts << QString("%1,%2,%3,%4,%5;").arg(m_handleName, pDspVar->Name()).arg(pDspVar->size()).arg(pDspVar->datatype()).arg(seg);
        const QStringList entry = var.split(",", Qt::SkipEmptyParts);
        QString memGroup = entry[0];
        QString varName = entry[1];
        int size = entry[2].toInt();
        int dataType = entry[3].toInt();
        int segment = entry[4].toInt();
        QJsonObject jsonVar;
        jsonVar.insert("Name", varName);
        jsonVar.insert("Size", size);
        jsonVar.insert("TypeData", dspVarDataTypeToJson(dataType));
        jsonVar.insert("Segment", dspVarSegmentToJson(segment));

        memGroupVariables[memGroup].append(jsonVar);
    }
    QJsonObject outData = inData;
    for(auto iter=memGroupVariables.constBegin(); iter!=memGroupVariables.constEnd(); iter++) {
        QJsonObject group = outData[iter.key()].toObject();
        group.insert("Variables", iter.value());
        outData.insert(iter.key(), group);
    }
    return outData;
}

QStringList TestDspInterface::dumpCycListItem()
{
    return d_ptr->getCyclicCmdList();
}

QString TestDspInterface::dspVarDataTypeToJson(int type)
{
    switch(type) {
    case DSPDATA::dFloat:
        return "DSPDATA::dFloat";
    case DSPDATA::dInt:
        return "DSPDATA::dInt";
    default:
        qFatal("Unknown DSP var type");
    }
}

QString TestDspInterface::dspVarSegmentToJson(int segment)
{
    switch(segment) {
    case DSPDATA::localSegment:
        return "DSPDATA::localSegment";
    case DSPDATA::globalSegment:
        return "DSPDATA::globalSegment";
    default:
        qFatal("Unknown DSP var segment");
    }
}

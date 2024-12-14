#include "testdspinterface.h"
#include <dspinterface_p.h>

TestDspInterface::TestDspInterface(QStringList valueNamesList) :
    m_valueNamesList(valueNamesList)
{
}

quint32 TestDspInterface::dspMemoryWrite(cDspMeasData *memgroup)
{
    struct TVarsWritten write = { memgroup->getName(), memgroup->getData() };
    m_valuesWritten.append(write);
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
        dump.insert("3-VarsWritten", dumpVariablesWritten());
    return dump;
}

QJsonObject TestDspInterface::dumpMemoryGroups()
{
    const QList<cDspMeasData*> dspMemoryDataList = d_ptr->getMemoryDataList();
    QJsonObject dumpMemGroup;
    for(cDspMeasData* memData : dspMemoryDataList) {
        QJsonObject entry;
        entry.insert("Size", int(memData->getSize()));
        entry.insert("UserMemSize", int(memData->getumemSize()));
        dumpMemGroup.insert(memData->getName(), entry);
    }
    return dumpMemGroup;
}

QJsonObject TestDspInterface::dumpVarList(QJsonObject inData)
{
    const QStringList varList = d_ptr->varList2String().split(";", Qt::SkipEmptyParts);
    QMap<QString, QJsonArray> memGroupVariables;
    for(const QString &var : varList) {
        // see: cDspMeasData::VarListLong
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

QJsonArray TestDspInterface::dumpVariablesWritten()
{
    QJsonArray arr;
    for(const TVarsWritten& entry : qAsConst(m_valuesWritten)) {
        QJsonObject values;
        for(int i=0; i<entry.dataWritten.count(); i++) {
            QString num = QString("0000%1").arg(i).right(4);
            QString key = QString("%1+%2").arg(entry.varName, num);
            values.insert(key,
                          QString("%1").arg(entry.dataWritten[i], -1, 'g', 6).toDouble());
        }
        arr.append(values);
    }
    return arr;
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

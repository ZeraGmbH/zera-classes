#include "testdspinterface.h"
#include <dspinterface_p.h>

TestDspInterface::TestDspInterface(QStringList valueNamesList, int entityId) :
    MockDspInterface(entityId),
    m_valueNamesList(valueNamesList)
{
}

quint32 TestDspInterface::dspMemoryWrite(cDspMeasData *memgroup)
{
    QString memGroupName = memgroup->getName();
    cDspMeasData *currentMemGroup = d_ptr->findMemHandle(memgroup->getName());
    bool nonEmptyMemgroupFound = false;
    if(currentMemGroup) {
        const QList<cDspVar*> currentVars = currentMemGroup->getVars();
        for(int var=0; var<currentVars.size(); ++var) {
            nonEmptyMemgroupFound = true;
            cDspVar* currentVar = currentVars[var];
            const QString varName = currentVar->Name();
            DSPDATA::dType dataType = static_cast<DSPDATA::dType>(currentVar->datatype());
            for(int varEntry=0; varEntry<currentVar->size(); ++varEntry) {
                const float* valuePointer = currentVar->data() + varEntry;
                QString label;
                if(varEntry == 0)
                    label = QString("%1:%2").arg(memGroupName, varName);
                else {
                    QString entryNum = QString("0000%1").arg(varEntry).right(4);
                    label = QString("%1:%2+%3").arg(memGroupName, varName, entryNum);
                }
                // There is this nasty float/uint cast
                // see cDspMeasData::setVarData & cDspMeasData::writeCommand()
                double value = 0.0;
                if(dataType == DSPDATA::dInt) {
                    uint* uintPointer = (uint*) valuePointer;
                    value = *uintPointer;
                }
                else
                    value = *valuePointer;
                struct TVarsWritten write = { m_transactionCount, label, value };
                m_valuesWritten.append(write);
            }
        }
    }
    if(!nonEmptyMemgroupFound) {
        const QVector<float> &values = memgroup->getData();
        struct TVarsWritten write = { m_transactionCount, QString("No variables found for memgroup %1").arg(memGroupName), float(qQNaN()) };
        m_valuesWritten.append(write);
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
        entry.insert("UserMemSize", int(memData->getUserMemSize()));
        dumpMemGroup.insert(memData->getName(), entry);
    }
    return dumpMemGroup;
}

QJsonObject TestDspInterface::dumpVarList(QJsonObject inData)
{
    const QStringList varList = d_ptr->varList2String(Zera::cDSPInterfacePrivate::PREPEND_NOTHING).split(";", Qt::SkipEmptyParts);
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

QJsonObject TestDspInterface::dumpVariablesWritten()
{
    QJsonObject values;
    for(int i=0; i<m_valuesWritten.count(); ++i) {
        const TVarsWritten& entry = m_valuesWritten[i];
        QString num = QString("0000%1").arg(entry.transcationCount).right(4);
        QString key = QString("WriteNo: %1 / Handle:Variable: %2").arg(num, entry.groupVarName);
        values.insert(key,
                      QString("%1").arg(entry.dataWritten, -1, 'g', 6).toDouble());
    }
    return values;
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

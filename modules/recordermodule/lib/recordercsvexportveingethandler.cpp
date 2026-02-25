#include "recordercsvexportveingethandler.h"

RecorderCsvExportVeinGetHandler::RecorderCsvExportVeinGetHandler(VeinStorage::StorageRecordDataPtr storedData) :
    m_storedData(storedData)
{
}

static const QString fieldSeparator = ";";
static const int floatPrecision = 8;

const QVariant &RecorderCsvExportVeinGetHandler::getCustomizedValue(const QVariant &currentValue)
{
    Q_UNUSED(currentValue)

    QString csv = getHeader() + "\n";

    const QStringList lines = getDataLines();
    if (!lines.isEmpty())
        csv += lines.join("\n") + "\n";

    m_csvExport = csv;
    return m_csvExport;
}

QString RecorderCsvExportVeinGetHandler::getHeader() const
{
    const VeinStorage::RecordEntityComponentSequence components = VeinStorage::RecordParamSequencer::toSequence(m_storedData->m_componentInfo);
    QString header = "Timestamp";
    for (const VeinStorage::RecordEntityComponent &component : components)
        header += fieldSeparator + component.m_component.m_label;
    return header;
}

QStringList RecorderCsvExportVeinGetHandler::getDataLines() const
{
    QStringList lines;
    QDateTime timestamp = m_storedData->m_timeOfFirstRecord;
    for (const VeinStorage::RecordEntryStored &entry : m_storedData->m_recordedData) {
        timestamp = timestamp.addMSecs(entry.m_timeDiffToFirstInMs);

        QString line = timestamp.toString("yyyy-MM-dd HH:mm:ss.zzz");
        for (float value : entry.m_values) {
            QString strValue = QString("%1").arg(value, 0, 'g', floatPrecision);
            line += fieldSeparator + strValue;
        }
        lines.append(line);
    }
    return lines;
}

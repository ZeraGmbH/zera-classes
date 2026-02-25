#ifndef RECORDERCSVEXPORTVEINGETHANDLER_H
#define RECORDERCSVEXPORTVEINGETHANDLER_H

#include <vs_abstractcomponentgetcustomizer.h>
#include <vs_storagerecordertypes.h>

class RecorderCsvExportVeinGetHandler : public AbstractComponentGetCustomizer
{
public:
    explicit RecorderCsvExportVeinGetHandler(VeinStorage::StorageRecordDataPtr storedData);
    const QVariant &getCustomizedValue(const QVariant &currentValue) override;
private:
    QString getHeader() const;
    QStringList getDataLines() const;

    VeinStorage::StorageRecordDataPtr m_storedData;
    QVariant m_csvExport;
};

#endif // RECORDERCSVEXPORTVEINGETHANDLER_H

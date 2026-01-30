#ifndef RECORDERJSONEXPORTVEINGETHANDLER_H
#define RECORDERJSONEXPORTVEINGETHANDLER_H

#include <vs_abstractcomponent.h>
#include <vs_storagerecordertypes.h>
#include <vs_abstractcomponentgetcustomizer.h>

class RecorderJsonExportVeinGetHandler : public AbstractComponentGetCustomizer
{
public:
    RecorderJsonExportVeinGetHandler(VeinStorage::StorageRecordDataPtr storedData);
    const QVariant &getCustomizedValue(const QVariant &currentValue) override;
private:
    VeinStorage::StorageRecordDataPtr m_storedData;
    QVariant m_jsonExport;
};

#endif // RECORDERJSONEXPORTVEINGETHANDLER_H

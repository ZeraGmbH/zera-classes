#ifndef TEST_ADJ_MODULE_OFFSET_H
#define TEST_ADJ_MODULE_OFFSET_H

#include <QObject>

class test_adj_module_offset : public QObject
{
    Q_OBJECT
private slots:
    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    // from here on with permission
    void denyNonOffsetChannel();
    void denyRangeNotSet();
};

#endif // TEST_ADJ_MODULE_OFFSET_H

#ifndef TEST_SOURCE_FULL_H
#define TEST_SOURCE_FULL_H

#include <QObject>

class test_source_full : public QObject
{
    Q_OBJECT
private slots:
    void entity_avail_mt310s2();
    void entity_avail_mt581s2();
    void vein_dump_mt310s2();
    void vein_dump_mt581s2();
};

#endif // TEST_SOURCE_FULL_H

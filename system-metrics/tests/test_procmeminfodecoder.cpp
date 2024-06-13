#include "test_procmeminfodecoder.h"
#include "procmeminfodecoder.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_procmeminfodecoder)

void test_procmeminfodecoder::initTestCase()
{
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfo");
}

void test_procmeminfodecoder::decodeSingleLineMeminfo()
{
    QString memInfoLine = "Active(file):           203772 kB";
    quint32 memValue = ProcMeminfoDecoder::decodeSingleMemoryValue(memInfoLine);
    QCOMPARE(memValue, 203772);
}

void test_procmeminfodecoder::decodeTestProcMeminfo()
{
    MemoryValues testMemoryValues = ProcMeminfoDecoder::getCurrentMemoryValues();
    QCOMPARE(testMemoryValues.totalMemory, 1000000);
    QCOMPARE(testMemoryValues.freeMemory, 400000);
    QCOMPARE(testMemoryValues.buffers, 150000);
    QCOMPARE(testMemoryValues.cached, 200000);
}


#include "test_jsonconfigfiles.h"
#include <QTest>

QTEST_MAIN(test_jsonconfigfiles)

void test_jsonconfigfiles::compareSessionLogConfigFileCountEqual()
{
    QStringList sessionFileList = QString(SESSION_FILES).split(",");
    QStringList loggerFileList = QString(LOGGER_CONFIG_FILES).split(",");
    QCOMPARE(sessionFileList.count(), loggerFileList.count());
}

static void toBaseName(QStringList &fileList)
{
    for(auto& file : fileList) {
        QFileInfo fi(file);
        file = fi.baseName();
    }
}

void test_jsonconfigfiles::compareSessionLogConfigFileBasenamesEqual()
{
    QStringList sessionFileList = QString(SESSION_FILES).split(",");
    sessionFileList.sort();
    toBaseName(sessionFileList);
    QStringList loggerFileList = QString(LOGGER_CONFIG_FILES).split(",");
    loggerFileList.sort();
    toBaseName(loggerFileList);
    QCOMPARE(sessionFileList, loggerFileList);
}

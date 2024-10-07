#include "test_lambda_config_load.h"
#include "lambdamoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_lambda_config_load)

void test_lambda_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_LAMBDAMODULE) + "/" + "com5003-lambdamodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_lambda_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_LAMBDAMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        LAMBDAMODULE::cLambdaModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}

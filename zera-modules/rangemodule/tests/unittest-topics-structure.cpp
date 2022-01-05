#include <gtest/gtest.h>
#include <QDir>
#include <QJsonDocument>
#include <zera-json-params-structure.h>

// Check all deviceinfos for validity by loading them into ZeraJsonParamsStructure
TEST(TEST_DEVICEINFO, ALL_VALID) {
    QDir dir(QStringLiteral("://topic"));
    QFileInfoList fileInfoList = dir.entryInfoList();
    EXPECT_FALSE(fileInfoList.isEmpty());
    for(auto fileInfo : fileInfoList) {
        QString diFileName = fileInfo.absoluteFilePath();
        QFile diFile(diFileName);
        bool diOpened = diFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
        EXPECT_EQ(diOpened, true) << "Topic could not be opened:" << qPrintable(diFileName);
        if(diOpened) {
            QJsonObject diStructure = QJsonDocument::fromJson(diFile.readAll()).object();
            ZeraJsonParamsStructure jsonParamStructure;
            ZeraJsonParamsStructure::ErrList errListStructure = jsonParamStructure.setJson(diStructure);
            QString errorInfoTxt;
            for(auto errInfo: errListStructure) {
                errorInfoTxt += "\n" + errInfo.strID() + ": " + errInfo.m_strInfo;
            }
            EXPECT_EQ(errListStructure.length(), 0) << "Topic invalid: " <<
                                                       qPrintable(diFileName) << qPrintable(errorInfoTxt) << "\n";
        }
    }
}

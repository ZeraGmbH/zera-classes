#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QTimer>
#include <QTemporaryFile>
#include "filedisappearwatcher.h"

static int disappearOrNotExistentCount = 0;

static void testDisappearEvents(int countCreate, int countRemove, int countNotExistent)
{
    if(countRemove > countCreate) {
        qFatal("countRemove > countCreate!!");
    }
    disappearOrNotExistentCount = 0;

    char const *p = "filedisappear-test";
    char const **dummyParam = { &p };
    int argc = 1;
    QCoreApplication loop(argc, const_cast<char**>(dummyParam));

    QTimer timerForEventLoopSetup;
    timerForEventLoopSetup.setSingleShot(true);
    QTimer timerForFinish;
    timerForFinish.setSingleShot(true);

    FileDisappearWatcher watcher;
    QObject::connect(&watcher, &FileDisappearWatcher::sigFileRemoved, [&](QString) {
        disappearOrNotExistentCount++;
    });

    QObject::connect( &timerForFinish, &QTimer::timeout, [&]() {
        loop.quit();
    });

    QVector<QTemporaryFile*> tempFiles(countCreate, nullptr);
    QObject::connect( &timerForEventLoopSetup, &QTimer::timeout, [&]() {
        // We have a running event loop here start our test
        for(int i=0; i<countCreate; i++) {
            tempFiles[i] = new QTemporaryFile;
            tempFiles[i]->open();
            watcher.watchFile(tempFiles[i]->fileName());
            if(countRemove) {
                countRemove--;
                delete tempFiles[i];
                tempFiles[i] = nullptr;
            }
        }
        for(int i=0; i<countNotExistent; i++) {
            QTemporaryFile* tmpFile = new QTemporaryFile;
            tmpFile->open();
            QString fileName = tmpFile->fileName();
            delete tmpFile;
            watcher.watchFile(fileName);
        }
        timerForFinish.start(200);
    });
    timerForEventLoopSetup.start(0);
    loop.exec();
    for(auto tempFileLeft : tempFiles) { // delete remaining tmp files
        delete tempFileLeft;
    }
}

TEST(TEST_FILE_DISAPPEAR_WATCHER, CREATE_DISAPPEAR) {
    testDisappearEvents(2, 2, 0);
    EXPECT_EQ(disappearOrNotExistentCount, 2);
}

TEST(TEST_FILE_DISAPPEAR_WATCHER, CREATE_ONLY) {
    testDisappearEvents(2, 0, 0);
    EXPECT_EQ(disappearOrNotExistentCount, 0);
}

TEST(TEST_FILE_DISAPPEAR_WATCHER, NOT_EXIST) {
    testDisappearEvents(0, 0, 2);
    EXPECT_EQ(disappearOrNotExistentCount, 2);
}

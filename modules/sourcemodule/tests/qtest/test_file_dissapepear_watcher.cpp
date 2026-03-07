#include "test_file_dissapepear_watcher.h"
#include "filedisappearwatcher.h"
#include <timemachineobject.h>
#include <QCoreApplication>
#include <QDir>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_file_dissapepear_watcher)

void test_file_dissapepear_watcher::init()
{
    QDir dir;
    dir.mkpath(getPathName());
}

void test_file_dissapepear_watcher::cleanup()
{
    QDir dir(getPathName());
    dir.removeRecursively();
}

void test_file_dissapepear_watcher::nonExistent()
{
    FileDisappearWatcher watcher;
    QSignalSpy spy(&watcher, &FileDisappearWatcher::sigFileRemoved);

    watcher.watchFile("foo");

    QCOMPARE(spy.count(), 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "foo");
}

void test_file_dissapepear_watcher::createTwoRemoveFirst()
{
    FileDisappearWatcher watcher;
    QSignalSpy spy(&watcher, &FileDisappearWatcher::sigFileRemoved);

    watcher.watchFile(createFile("one"));
    watcher.watchFile(createFile("two"));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);

    deleteFile("one");

    QCOMPARE(spy.count(), 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], getFilePath("one"));
}

void test_file_dissapepear_watcher::createTwoRemoveSecond()
{
    FileDisappearWatcher watcher;
    QSignalSpy spy(&watcher, &FileDisappearWatcher::sigFileRemoved);

    watcher.watchFile(createFile("one"));
    watcher.watchFile(createFile("two"));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);

    deleteFile("two");

    QCOMPARE(spy.count(), 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], getFilePath("two"));
}

void test_file_dissapepear_watcher::createTwoRemoveBoth()
{
    FileDisappearWatcher watcher;
    QSignalSpy spy(&watcher, &FileDisappearWatcher::sigFileRemoved);

    watcher.watchFile(createFile("one"));
    watcher.watchFile(createFile("two"));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);

    // Sequence is not predictable => feedEventLoop() after each delete
    deleteFile("one");
    TimeMachineObject::feedEventLoop();
    deleteFile("two");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy[0][0], getFilePath("one"));
    QCOMPARE(spy[1][0], getFilePath("two"));
}

void test_file_dissapepear_watcher::createTwoRemoveFirstRecreateAndRemove()
{
    FileDisappearWatcher watcher;
    QSignalSpy spy(&watcher, &FileDisappearWatcher::sigFileRemoved);

    watcher.watchFile(createFile("one"));
    watcher.watchFile(createFile("two"));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);

    deleteFile("one");

    QCOMPARE(spy.count(), 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], getFilePath("one"));

    spy.clear();
    createFile("one");
    deleteFile("one");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);
}

void test_file_dissapepear_watcher::createTwoRemoveFirstRecreateTrackAndRemove()
{
    FileDisappearWatcher watcher;
    QSignalSpy spy(&watcher, &FileDisappearWatcher::sigFileRemoved);

    watcher.watchFile(createFile("one"));
    watcher.watchFile(createFile("two"));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);

    deleteFile("one");

    QCOMPARE(spy.count(), 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], getFilePath("one"));

    spy.clear();
    watcher.watchFile(createFile("one"));
    deleteFile("one");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
}

void test_file_dissapepear_watcher::createOneTrackTwiceRemove()
{
    FileDisappearWatcher watcher;
    QSignalSpy spy(&watcher, &FileDisappearWatcher::sigFileRemoved);

    const QString filePath = createFile("one");
    watcher.watchFile(filePath);
    watcher.watchFile(filePath);

    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);

    deleteFile("one");

    QCOMPARE(spy.count(), 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], getFilePath("one"));
}

QString test_file_dissapepear_watcher::getPathName()
{
    return "/tmp/" + QCoreApplication::applicationName();
}

QString test_file_dissapepear_watcher::getFilePath(const QString &fileName)
{
    return getPathName() + "/" + fileName;
}

QString test_file_dissapepear_watcher::createFile(const QString &fileName)
{
    QString filePath = getFilePath(fileName);
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly))
        qCritical("Could not create file %s!", qPrintable(file.fileName()));
    file.close();
    return filePath;
}

bool test_file_dissapepear_watcher::deleteFile(const QString &fileName)
{
    QString filePath = getPathName() + "/" + fileName;
    QFile file(filePath);
    return file.remove();
}

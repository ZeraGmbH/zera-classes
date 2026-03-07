#ifndef TEST_FILE_DISSAPEPEAR_WATCHER_H
#define TEST_FILE_DISSAPEPEAR_WATCHER_H

#include <QObject>

class test_file_dissapepear_watcher : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void nonExistent();
    void createTwoRemoveFirst();
    void createTwoRemoveSecond();
    void createTwoRemoveBoth();
    void createTwoRemoveFirstRecreateAndRemove();
    void createTwoRemoveFirstRecreateTrackAndRemove();
    void createOneTrackTwiceRemove();

private:
    static QString getPathName();
    QString getFilePath(const QString &fileName);
    QString createFile(const QString &fileName);
    bool deleteFile(const QString &fileName);
};

#endif // TEST_FILE_DISSAPEPEAR_WATCHER_H

#ifndef cFileDisappearWatcher_H
#define cFileDisappearWatcher_H

#include <QObject>
#include <QFileSystemWatcher>

class cFileDisappearWatcher : public QObject
{
    Q_OBJECT
public:
    explicit cFileDisappearWatcher(QObject *parent = nullptr);
    void watchFile(const QString fileName);
    void resetFiles();
signals:
    void sigFileRemoved(const QString fileName);

private slots:
    void onFileChanged(const QString &fileName);
private:
    QFileSystemWatcher m_fileWatcher;
signals:
    void sigFileRemovedToQueue(const QString fileName);
};

#endif // cFileDisappearWatcher_H

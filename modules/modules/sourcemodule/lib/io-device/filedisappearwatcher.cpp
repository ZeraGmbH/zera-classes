#include <QFile>
#include "filedisappearwatcher.h"

FileDisappearWatcher::FileDisappearWatcher(QObject *parent) : QObject(parent)
{
    connect(this, &FileDisappearWatcher::sigFileRemovedToQueue, this, &FileDisappearWatcher::sigFileRemoved, Qt::QueuedConnection);
    connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &FileDisappearWatcher::onFileChanged);
}

void FileDisappearWatcher::watchFile(const QString fileName)
{
    bool fileOK = false;
    if(QFile::exists(fileName)) {
        if(m_fileWatcher.addPath(fileName)) {
            fileOK = true;
        }
    }
    if(!fileOK) {
        emit sigFileRemovedToQueue(fileName);
    }
}

void FileDisappearWatcher::resetFiles()
{
    QStringList watchedFiles = m_fileWatcher.files();
    for(auto file : watchedFiles) {
        m_fileWatcher.removePath(file);
    }
}

void FileDisappearWatcher::onFileChanged(const QString &fileName)
{
    if(!QFile::exists(fileName)) {
        m_fileWatcher.removePath(fileName);
        emit sigFileRemovedToQueue(fileName);
    }
}

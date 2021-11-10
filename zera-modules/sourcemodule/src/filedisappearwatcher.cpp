#include <QFile>
#include "filedisappearwatcher.h"

cFileDisappearWatcher::cFileDisappearWatcher(QObject *parent) : QObject(parent)
{
    connect(this, &cFileDisappearWatcher::sigFileRemovedToQueue, this, &cFileDisappearWatcher::sigFileRemoved, Qt::QueuedConnection);
    connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &cFileDisappearWatcher::onFileChanged);
}

void cFileDisappearWatcher::watchFile(const QString fileName)
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

void cFileDisappearWatcher::resetFiles()
{
    QStringList watchedFiles = m_fileWatcher.files();
    for(auto file : watchedFiles) {
        m_fileWatcher.removePath(file);
    }
}

void cFileDisappearWatcher::onFileChanged(const QString &fileName)
{
    if(!QFile::exists(fileName)) {
        m_fileWatcher.removePath(fileName);
        emit sigFileRemovedToQueue(fileName);
    }
}

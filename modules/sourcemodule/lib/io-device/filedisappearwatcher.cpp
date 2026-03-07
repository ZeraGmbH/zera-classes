#include <QFile>
#include "filedisappearwatcher.h"

FileDisappearWatcher::FileDisappearWatcher(QObject *parent) : QObject(parent)
{
    connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &FileDisappearWatcher::onFileChanged);
}

void FileDisappearWatcher::watchFile(const QString &fileName)
{
    bool fileOK = false;
    if(QFile::exists(fileName)) {
        if(m_fileWatcher.addPath(fileName))
            fileOK = true;
    }
    if(!fileOK)
        emit emitSigFileRemoved(fileName);
}

void FileDisappearWatcher::resetFiles()
{
    const QStringList watchedFiles = m_fileWatcher.files();
    for(const QString &file : watchedFiles) {
        m_fileWatcher.removePath(file);
    }
}

void FileDisappearWatcher::onFileChanged(const QString &fileName)
{
    if(!QFile::exists(fileName)) {
        m_fileWatcher.removePath(fileName);
        emit emitSigFileRemoved(fileName);
    }
}

void FileDisappearWatcher::emitSigFileRemoved(const QString &fileName)
{
    QMetaObject::invokeMethod(this,
                              "sigFileRemoved",
                              Qt::QueuedConnection,
                              Q_ARG(const QString &, fileName));

}

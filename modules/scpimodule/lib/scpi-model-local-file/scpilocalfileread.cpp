#include "scpilocalfileread.h"
#include "scpimodellocalfile.h"
#include "scpilocalfileerrormessages.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>

namespace SCPIMODULE
{

static constexpr qint64 MaxLocalFileSize = 1024 * 1024; // 1 MiB

QString ScpiLocalFileRead::readLocalFileContent(const QString &baseDirPath, const QString &fileName)
{
    // the file path must be relative to our base directory - no absolute paths allowed
    if (fileName.isEmpty() || fileName.startsWith('/') || QFileInfo(fileName).isAbsolute())
        return ScpiLocalFileErrorMessages::InvalidFilePath;

    // no '..' segments allowed so we can never leave the base directory
    const QStringList pathSegments = fileName.split('/');
    for (const QString &pathSegment : pathSegments) {
        if (pathSegment == "..")
            return ScpiLocalFileErrorMessages::InvalidFilePath;
    }

    // resolve relative to our base directory (lexical normalization only - no symlinks resolved yet)
    const QString fullPath = QDir(baseDirPath).absoluteFilePath(fileName);
    const QFileInfo fileInfo(fullPath);
    if (!fileInfo.exists())
        return ScpiLocalFileErrorMessages::FileNotFound;

    // also catch escapes via symbolic links pointing outside of our base directory
    const QString canonicalFile = fileInfo.canonicalFilePath();
    const QString canonicalBase = QFileInfo(baseDirPath).canonicalFilePath();
    if (canonicalBase.isEmpty() || !canonicalFile.startsWith(canonicalBase + QDir::separator()))
        return ScpiLocalFileErrorMessages::InvalidFilePath;

    if (!fileInfo.isFile())
        return ScpiLocalFileErrorMessages::NotARegularFile;

    if (fileInfo.size() > MaxLocalFileSize)
        return ScpiLocalFileErrorMessages::FileTooLarge;

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly))
        return ScpiLocalFileErrorMessages::CannotOpenFile;

    const QByteArray fileData = file.readAll();
    if (!ScpiModelLocalFile::isValidUtf8(fileData))
        return ScpiLocalFileErrorMessages::InvalidUtf8;

    return QString::fromUtf8(fileData);
}

}
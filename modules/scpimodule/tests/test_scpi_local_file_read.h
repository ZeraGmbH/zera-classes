#ifndef TEST_SCPI_LOCAL_FILE_READ_H
#define TEST_SCPI_LOCAL_FILE_READ_H

#include <QObject>

class test_scpi_local_file_read : public QObject
{
    Q_OBJECT
private slots:
    void readSimpleFile();
    void readExampleFileWithUmlautsAndChineseText();
    void readFileInSubdirectory();
    void rejectPathTraversalWithDotDot();
    void rejectAbsoluteFilePaths();
    void rejectSymlinkEscapingBaseDirectory();
    void allowSymlinkInsideBaseDirectory();
    void rejectBrokenSymlink();
    void rejectWrongFileName();
    void rejectSubdirectorySelected();
    void rejectEmptyFileName();
    void rejectFileTooLarge();
    void rejectInvalidUtf8File();
};

#endif // TEST_SCPI_LOCAL_FILE_READ_H

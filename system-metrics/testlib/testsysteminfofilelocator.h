#ifndef TESTSYSTEMINFOFILELOCATOR_H
#define TESTSYSTEMINFOFILELOCATOR_H

#include <systeminfofilelocator.h>

class TestSystemInfoFileLocator : public SystemInfoFileLocator
{
public:
    static void setProcStatusFileName(QString procStatusFileName);
};

#endif // TESTSYSTEMINFOFILELOCATOR_H

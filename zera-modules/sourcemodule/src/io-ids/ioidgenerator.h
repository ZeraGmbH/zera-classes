#ifndef CSOURCEIDGENERATOR_H
#define CSOURCEIDGENERATOR_H

class IoIdGenerator
{
public:
    int nextID();
    int nextIDStatic();
private:
    int m_currentID = -1;
    static int m_currentIDStatic;
};

#endif // CSOURCEIDGENERATOR_H

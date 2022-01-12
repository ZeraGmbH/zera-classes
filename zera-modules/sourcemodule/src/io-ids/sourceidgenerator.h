#ifndef CSOURCEIDGENERATOR_H
#define CSOURCEIDGENERATOR_H


class SourceIdGenerator
{
public:
    int nextID();
private:
    int m_currentID = -1;
};

#endif // CSOURCEIDGENERATOR_H

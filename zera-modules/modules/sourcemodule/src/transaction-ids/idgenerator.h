#ifndef CSOURCEIDGENERATOR_H
#define CSOURCEIDGENERATOR_H

class IoIdGenerator
{
public:
    IoIdGenerator() = default;
    IoIdGenerator(const IoIdGenerator&) = delete;
    int nextID();
private:
    int m_currentID = -1;
};

#endif // CSOURCEIDGENERATOR_H

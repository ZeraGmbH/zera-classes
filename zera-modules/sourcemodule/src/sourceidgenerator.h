#ifndef CSOURCEIDGENERATOR_H
#define CSOURCEIDGENERATOR_H


class cSourceIdGenerator
{
public:
    /**
     * @brief nextID
     * @return unique value != 0
     */
    int nextID();
private:
    int m_currentID = -1;
};

#endif // CSOURCEIDGENERATOR_H

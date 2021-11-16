#ifndef CSOURCETRANSACTIONIDGENERATOR_H
#define CSOURCETRANSACTIONIDGENERATOR_H


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

#endif // CSOURCETRANSACTIONIDGENERATOR_H

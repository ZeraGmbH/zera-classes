#ifndef CSOURCETRANSACTIONIDGENERATOR_H
#define CSOURCETRANSACTIONIDGENERATOR_H


class cSourceTransactionIdGenerator
{
public:
    /**
     * @brief nextTransactionID
     * @return unique value != 0
     */
    int nextTransactionID();
private:
    int m_currentTransActionID = -1;
};

#endif // CSOURCETRANSACTIONIDGENERATOR_H

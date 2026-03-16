#ifndef TEST_INTTOHEXSTRINGCONVERT_H
#define TEST_INTTOHEXSTRINGCONVERT_H

#include <QObject>

class test_inttohexstringconvert : public QObject
{
    Q_OBJECT
private slots:
    void digitCount4();
    void digitCount6();
    void leadingZeros0Val();
    void heading0x();
    void valOneByte();
    void upperChars();
    void moreDigitsThanMaxDigits();
};

#endif // TEST_INTTOHEXSTRINGCONVERT_H

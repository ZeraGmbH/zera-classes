#ifndef TEST_ADJ_MODULE_GAIN_H
#define TEST_ADJ_MODULE_GAIN_H

#include <QObject>

class test_adj_module_gain : public QObject
{
    Q_OBJECT
private slots:
    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    // from here on with permission
    void outOfLimitLower();
    void outOfLimitUpper();

    void oneNodeWithinLimitLower();
    void oneNodeWithinLimitUpper();

    void twoNodesCheckNodesGenerated();
    void oneNodeCheckCorrectionsCalculated();
    void twoNodesCheckCorrectionsCalculatedLinear();
    void twoNodesCheckCorrectionsCalculatedNonLinear();

    void denyRangeNotSet();
};

#endif // TEST_ADJ_MODULE_GAIN_H

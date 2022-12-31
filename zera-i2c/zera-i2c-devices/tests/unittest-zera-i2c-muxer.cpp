#include "i2cmuxerscopedonoff.h"
#include "i2cmuxernull.h"
#include <gtest/gtest.h>

class I2cMuxerTest : public I2cMuxerInterface
{
public:
    I2cMuxerTest(QString devName) { m_devName = devName; }
    void enableMuxChannel() override { m_enableCount++; }
    void disableMux() override { m_enableCount--; }
    virtual QString getDevIdString() override { return m_devName; }
    int getEnableCount() { return m_enableCount; }
private:
    int m_enableCount = 0;
    QString m_devName;
};


TEST(TEST_I2CMUX_SCOPED_ON_OFF, I2cMuxerTest) {
    I2cMuxerTest *i2cMuxerTest = new I2cMuxerTest("foo");
    I2cMuxerInterface::Ptr i2cMuxer = I2cMuxerInterface::Ptr(i2cMuxerTest);

    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 0);
    i2cMuxer->enableMuxChannel();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
    i2cMuxer->enableMuxChannel();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 2);
    i2cMuxer->disableMux();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
    i2cMuxer->disableMux();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 0);
}

TEST(TEST_I2CMUX_SCOPED_ON_OFF, I2cMuxerScopedOnOff) {
    I2cMuxerTest *i2cMuxerTest = new I2cMuxerTest("foo");
    I2cMuxerInterface::Ptr i2cMuxer = I2cMuxerInterface::Ptr(i2cMuxerTest);
    {
        I2cMuxerScopedOnOff i2cMuxerScoped(i2cMuxer);
        EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
    }
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 0);
}

TEST(TEST_I2CMUX_SCOPED_ON_OFF, I2cMuxerScopedOnOffNested) {
    I2cMuxerTest *i2cMuxerTest = new I2cMuxerTest("foo");
    I2cMuxerInterface::Ptr i2cMuxer = I2cMuxerInterface::Ptr(i2cMuxerTest);
    {
        I2cMuxerScopedOnOff i2cMuxerScoped1(i2cMuxer);
        EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
        {
            I2cMuxerScopedOnOff i2cMuxerScoped2(i2cMuxer);
            EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
        }
        EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
    }
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 0);
}

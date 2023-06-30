#include "gtest/gtest.h"
#include <QObject>

int main(int argc, char **argv) {
    Q_INIT_RESOURCE(resource);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

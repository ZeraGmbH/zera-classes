#include <gtest/gtest.h>
#include <QFile>
#include "sourcepersistentjsonstate.h"

TEST(TEST_SOURCE_PERSISTENT_STATE, STATE_STRUCT_GEN) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        SourcePersistentJsonState* stateIo = new SourcePersistentJsonState(SupportedSourceTypes(type), QString(), QString());
        EXPECT_EQ(stateIo->getJsonStructure(), cSourceJsonStructureLoader::getJsonStructure(SupportedSourceTypes(type)));
    }
}

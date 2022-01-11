#include <gtest/gtest.h>
#include <QFile>
#include "json/persistentjsonstate.h"
#include "json/jsonstructureloader.h"

TEST(TEST_SOURCE_PERSISTENT_STATE, STATE_STRUCT_GEN) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        SourcePersistentJsonState* stateIo = new SourcePersistentJsonState(SupportedSourceTypes(type), QString(), QString());
        EXPECT_EQ(stateIo->getJsonStructure(), JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type)));
    }
}

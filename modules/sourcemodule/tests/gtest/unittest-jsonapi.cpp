#include <gtest/gtest.h>
#include <zera-json-params-state.h>
#include "supportedsources.h"
#include "jsonstructapi.h"
#include "jsonparamapi.h"
#include "jsonstructureloader.h"

static void echoStructureName(int type)
{
    Q_UNUSED(type)
    //qInfo("Checking structure file %s...", qPrintable(JsonFilenames::getJsonStructurePath(SupportedSourceTypes(type))));
}

TEST(TEST_SOURCE_STRUCT_API, NAME_VALID) { // check if all structure files have a name entry
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        echoStructureName(type);
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        QString key = "Name";
        EXPECT_EQ(structure.contains(key), true);
        EXPECT_EQ(structure[key].isString(), true);
        EXPECT_EQ(structure[key].toString().isEmpty(), false);
    }
}

TEST(TEST_SOURCE_STRUCT_API, PHASE_COUNT_U_VALID) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        echoStructureName(type);
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        QString key = "UPhaseMax";
        EXPECT_EQ(structure.contains(key), true);
        JsonStructApi jsonApi(structure);
        EXPECT_EQ(structure[key].toInt(-1), jsonApi.getCountUPhases());
    }
}

TEST(TEST_SOURCE_STRUCT_API, PHASE_COUNT_I_VALID) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        echoStructureName(type);
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        QString key = "IPhaseMax";
        EXPECT_EQ(structure.contains(key), true);
        JsonStructApi jsonApi(structure);
        EXPECT_EQ(structure[key].toInt(-1), jsonApi.getCountIPhases());
    }
}

TEST(TEST_SOURCE_STRUCT_API, PHASE_COUNT_IO_PREFIX_VALID) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        echoStructureName(type);
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        QString key = "IoPrefix";
        EXPECT_EQ(structure.contains(key), true);
        JsonStructApi jsonApi(structure);
        EXPECT_EQ(structure[key].toString().toLatin1(), jsonApi.getIoPrefix());
    }
}


TEST(TEST_SOURCE_PARAM_API, ON_AVAIL_AND_OFF) { // check if all structure files have 'on":false
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        echoStructureName(type);
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        ZeraJsonParamsState jsonParamsState;
        jsonParamsState.setStructure(structure);
        QJsonObject defaultParams = jsonParamsState.getDefaultJsonState();

        QString key = "on";
        EXPECT_EQ(defaultParams.contains(key), true);
        EXPECT_EQ(defaultParams[key].type(), QJsonValue::Bool);
    }
}

TEST(TEST_SOURCE_PARAM_API, GETTER_SETTER_ON) { // initial off (see ON_AVAIL_AND_OFF) -> on -> off
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        echoStructureName(type);
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        ZeraJsonParamsState jsonParamsState;
        jsonParamsState.setStructure(structure);
        QJsonObject defaultParams = jsonParamsState.getDefaultJsonState();

        JsonParamApi paramApi;
        paramApi.setParams(defaultParams);
        EXPECT_EQ(paramApi.getOn(), false);

        paramApi.setOn(true);
        QJsonObject changedParams = paramApi.getParams();
        QString key = "on";
        EXPECT_EQ(changedParams[key].type(), QJsonValue::Bool);
        EXPECT_EQ(changedParams[key].toBool(), true);

        paramApi.setParams(changedParams);
        paramApi.setOn(false);
        changedParams = paramApi.getParams();
        EXPECT_EQ(changedParams[key].type(), QJsonValue::Bool);
        EXPECT_EQ(changedParams[key].toBool(), false);
    }
}

TEST(TEST_SOURCE_PARAM_API, PHASE_PARAM_AVAIL_IN_DEFAULT) { // check if all generated default states contain mandatory items
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        echoStructureName(type);
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        JsonStructApi structApi = JsonStructApi(structure);
        ZeraJsonParamsState jsonParamsState;
        jsonParamsState.setStructure(structure);
        QJsonObject defaultParams = jsonParamsState.getDefaultJsonState();
        QStringList phaseNames = QStringList() << "U" << "I";
        for(auto uI: phaseNames) {
            int countPhases = uI == "U" ? structApi.getCountUPhases() :structApi.getCountIPhases();
            for(int phase=1; phase<=countPhases; phase++) {
                QString phaseName = QString("%1%2").arg(uI).arg(phase);
                EXPECT_EQ(defaultParams.contains(phaseName), true);
                EXPECT_EQ(defaultParams[phaseName].isObject(), true);
                EXPECT_EQ(defaultParams[phaseName].toObject().contains("rms"), true);
                EXPECT_EQ(defaultParams[phaseName].toObject()["rms"].isDouble(), true);
                EXPECT_EQ(defaultParams[phaseName].toObject().contains("angle"), true);
                EXPECT_EQ(defaultParams[phaseName].toObject()["angle"].isDouble(), true);
                EXPECT_EQ(defaultParams[phaseName].toObject().contains("on"), true);
                EXPECT_EQ(defaultParams[phaseName].toObject()["on"].isBool(), true);
            }
        }

        QString key = "Frequency";
        EXPECT_EQ(defaultParams.contains(key), true);
        EXPECT_EQ(defaultParams[key].isObject(), true);
        EXPECT_EQ(defaultParams[key].toObject().contains("type"), true);
        EXPECT_EQ(defaultParams[key].toObject()["type"].isString(), true);
        QString freqType = defaultParams[key].toObject()["type"].toString();
        EXPECT_EQ(freqType == "var" || freqType == "sync", true);
        EXPECT_EQ(defaultParams[key].toObject().contains("val"), true);
        EXPECT_EQ(defaultParams[key].toObject()["val"].isDouble(), true);
    }
}

TEST(TEST_SOURCE_PARAM_API, GETTER_RMS) {
    QJsonObject val1 {{"rms", 1.1}};
    QJsonObject val2 {{"rms", 2.2}};
    QJsonObject val3 {{"rms", 3.3}};
    QJsonObject testParams
    {
        {"U1", val1},
        {"U2", val2},
        {"I3", val3}
    };
    JsonParamApi paramApi;
    paramApi.setParams(testParams);
    EXPECT_DOUBLE_EQ(paramApi.getRms(phaseType::U, 0), 1.1);
    EXPECT_DOUBLE_EQ(paramApi.getRms(phaseType::U, 1), 2.2);
    EXPECT_DOUBLE_EQ(paramApi.getRms(phaseType::I, 2), 3.3);
}

TEST(TEST_SOURCE_PARAM_API, GETTER_ANGLE) {
    QJsonObject val1 {{"angle", 0.1}};
    QJsonObject val2 {{"angle", 0.2}};
    QJsonObject val3 {{"angle", 0.3}};
    QJsonObject testParams
    {
        {"U1", val1},
        {"U2", val2},
        {"I3", val3}
    };
    JsonParamApi paramApi;
    paramApi.setParams(testParams);
    EXPECT_DOUBLE_EQ(paramApi.getAngle(phaseType::U, 0), 0.1);
    EXPECT_DOUBLE_EQ(paramApi.getAngle(phaseType::U, 1), 0.2);
    EXPECT_DOUBLE_EQ(paramApi.getAngle(phaseType::I, 2), 0.3);
}

TEST(TEST_SOURCE_PARAM_API, GETTER_PHASE_ON) {
    QJsonObject val1 {{"on", false}};
    QJsonObject val2 {{"on", true}};
    QJsonObject val3 {{"on", false}};
    QJsonObject testParams
    {
        {"U1", val1},
        {"U2", val2},
        {"I3", val3}
    };
    JsonParamApi paramApi;
    paramApi.setParams(testParams);
    EXPECT_EQ(paramApi.getOn(phaseType::U, 0), false);
    EXPECT_EQ(paramApi.getOn(phaseType::U, 1), true);
    EXPECT_EQ(paramApi.getOn(phaseType::I, 2), false);
}

TEST(TEST_SOURCE_PARAM_API, GETTER_FREQ_VAR) {
    QJsonObject val1 {{"type", "var"}};
    QJsonObject testParams
    {
        {"Frequency", val1}
    };
    JsonParamApi paramApi;
    paramApi.setParams(testParams);
    EXPECT_EQ(paramApi.getFreqVarOn(), true);
}

TEST(TEST_SOURCE_PARAM_API, GETTER_FREQ_SYNC) {
    QJsonObject val1 {{"type", "sync"}};
    QJsonObject testParams
    {
        {"Frequency", val1}
    };
    JsonParamApi paramApi;
    paramApi.setParams(testParams);
    EXPECT_EQ(paramApi.getFreqVarOn(), false);
}

TEST(TEST_SOURCE_PARAM_API, GETTER_FREQ_VAL) {
    QJsonObject val1 {{"val", 50.001}};
    QJsonObject testParams
    {
        {"Frequency", val1}
    };
    JsonParamApi paramApi;
    paramApi.setParams(testParams);
    EXPECT_DOUBLE_EQ(paramApi.getFreqVal(), 50.001);
}


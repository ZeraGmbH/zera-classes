#include <gtest/gtest.h>
#include "sourcezerastatequeryevaluator.h"

TEST(TEST_QUERY_ZERA, STATE_QUERY_EMPTY) {
    IIoQueryContentEvaluator::Ptr evalStateQuery = IIoQueryContentEvaluator::Ptr(new SourceZeraStateQueryEvaluator);
    EXPECT_TRUE(evalStateQuery->evalQueryResponse(""));
}

TEST(TEST_QUERY_ZERA, STATE_QUERY_INVALID) {
    IIoQueryContentEvaluator::Ptr evalStateQuery = IIoQueryContentEvaluator::Ptr(new SourceZeraStateQueryEvaluator);
    EXPECT_FALSE(evalStateQuery->evalQueryResponse("0010"));
}

TEST(TEST_QUERY_ZERA, STATE_QUERY_VALID) {
    IIoQueryContentEvaluator::Ptr evalStateQuery = IIoQueryContentEvaluator::Ptr(new SourceZeraStateQueryEvaluator);
    EXPECT_TRUE(evalStateQuery->evalQueryResponse("0000"));
}

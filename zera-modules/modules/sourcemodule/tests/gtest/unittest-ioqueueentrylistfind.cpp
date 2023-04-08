#include <gtest/gtest.h>
#include "ioqueuegrouplistfind.h"

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_IDX_EMPTY) {
    IoQueueGroupListPtr list;
    EXPECT_EQ(IoQueueGroupListFind::findGroupIdx(list, 0), -1);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_GROUP_EMPTY) {
    IoQueueGroupListPtr list;
    EXPECT_EQ(IoQueueGroupListFind::findGroup(list, 0), nullptr);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_IDX_WRONG_ID) {
    IoQueueGroupListPtr list;
    IoQueueGroup::Ptr entry = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id = entry->getGroupId();
    list.append(entry);
    EXPECT_EQ(IoQueueGroupListFind::findGroupIdx(list, id+1), -1);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_GROUP_WRONG_ID) {
    IoQueueGroupListPtr list;
    IoQueueGroup::Ptr entry = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id = entry->getGroupId();
    list.append(entry);
    EXPECT_EQ(IoQueueGroupListFind::findGroup(list, id+1), nullptr);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, FIND_VALID_IDX) {
    IoQueueGroupListPtr list;

    IoQueueGroup::Ptr entry1 = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id1 = entry1->getGroupId();
    list.append(entry1);

    IoQueueGroup::Ptr entry2 = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id2 = entry2->getGroupId();
    list.append(entry2);

    EXPECT_EQ(IoQueueGroupListFind::findGroupIdx(list, id1), 0);
    EXPECT_EQ(IoQueueGroupListFind::findGroupIdx(list, id2), 1);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, FIND_VALID_GROUP) {
    IoQueueGroupListPtr list;

    IoQueueGroup::Ptr entry1 = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id1 = entry1->getGroupId();
    list.append(entry1);

    IoQueueGroup::Ptr entry2 = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id2 = entry2->getGroupId();
    list.append(entry2);

    EXPECT_EQ(IoQueueGroupListFind::findGroup(list, id1).get(), entry1.get());
    EXPECT_EQ(IoQueueGroupListFind::findGroup(list, id2).get(), entry2.get());
}

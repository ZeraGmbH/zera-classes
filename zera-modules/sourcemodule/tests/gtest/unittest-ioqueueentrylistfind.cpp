#include <gtest/gtest.h>
#include "io-queue/ioqueueentrylist.h"

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_IDX_EMPTY) {
    IoQueueEntryList list;
    EXPECT_EQ(IoQueueEntryListFind::findGroupIdx(list, 0), -1);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_GROUP_EMPTY) {
    IoQueueEntryList list;
    EXPECT_EQ(IoQueueEntryListFind::findGroup(list, 0), nullptr);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_IDX_WRONG_ID) {
    IoQueueEntryList list;
    IoQueueEntry::Ptr entry = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id = entry->getGroupId();
    list.append(entry);
    EXPECT_EQ(IoQueueEntryListFind::findGroupIdx(list, id+1), -1);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, NO_FIND_GROUP_WRONG_ID) {
    IoQueueEntryList list;
    IoQueueEntry::Ptr entry = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id = entry->getGroupId();
    list.append(entry);
    EXPECT_EQ(IoQueueEntryListFind::findGroup(list, id+1), nullptr);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, FIND_VALID_IDX) {
    IoQueueEntryList list;

    IoQueueEntry::Ptr entry1 = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id1 = entry1->getGroupId();
    list.append(entry1);

    IoQueueEntry::Ptr entry2 = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id2 = entry2->getGroupId();
    list.append(entry2);

    EXPECT_EQ(IoQueueEntryListFind::findGroupIdx(list, id1), 0);
    EXPECT_EQ(IoQueueEntryListFind::findGroupIdx(list, id2), 1);
}

TEST(TEST_IO_QUEUE_ENTRY_LIST, FIND_VALID_GROUP) {
    IoQueueEntryList list;

    IoQueueEntry::Ptr entry1 = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id1 = entry1->getGroupId();
    list.append(entry1);

    IoQueueEntry::Ptr entry2 = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    int id2 = entry2->getGroupId();
    list.append(entry2);

    EXPECT_EQ(IoQueueEntryListFind::findGroup(list, id1).get(), entry1.get());
    EXPECT_EQ(IoQueueEntryListFind::findGroup(list, id2).get(), entry2.get());
}

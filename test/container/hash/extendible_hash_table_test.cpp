/**
 * extendible_hash_test.cpp
 */

#include <memory>
#include <thread>  // NOLINT

#include "container/hash/extendible_hash_table.h"
#include "gtest/gtest.h"
// #include <random>

namespace bustub {

TEST(ExtendibleHashTableTest, SampleTest) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(2);

  table->Insert(1, "a");
  table->Insert(2, "b");
  table->Insert(3, "c");
  table->Insert(4, "d");
  table->Insert(5, "e");
  table->Insert(6, "f");
  table->Insert(7, "g");
  table->Insert(8, "h");
  table->Insert(9, "i");
  EXPECT_EQ(2, table->GetLocalDepth(0));
  EXPECT_EQ(3, table->GetLocalDepth(1));
  EXPECT_EQ(2, table->GetLocalDepth(2));
  EXPECT_EQ(2, table->GetLocalDepth(3));

  std::string result;
  table->Find(9, result);
  EXPECT_EQ("i", result);
  table->Find(8, result);
  EXPECT_EQ("h", result);
  table->Find(2, result);
  EXPECT_EQ("b", result);
  EXPECT_FALSE(table->Find(10, result));

  EXPECT_TRUE(table->Remove(8));
  EXPECT_TRUE(table->Remove(4));
  EXPECT_TRUE(table->Remove(1));
  EXPECT_FALSE(table->Remove(20));
}

TEST(ExtendibleHashTableTest, ConcurrentInsertTest) {
  const int num_runs = 50;
  const int num_threads = 3;

  // Run concurrent test multiple times to guarantee correctness.
  for (int run = 0; run < num_runs; run++) {
    auto table = std::make_unique<ExtendibleHashTable<int, int>>(2);
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int tid = 0; tid < num_threads; tid++) {
      threads.emplace_back([tid, &table]() { table->Insert(tid, tid); });
    }
    for (int i = 0; i < num_threads; i++) {
      threads[i].join();
    }

    EXPECT_EQ(table->GetGlobalDepth(), 1);
    for (int i = 0; i < num_threads; i++) {
      int val;
      EXPECT_TRUE(table->Find(i, val));
      EXPECT_EQ(i, val);
    }
  }
}

// TEST(ExtendibleHashTest, RandomInsertAndDeleteTest) {
//   // set leaf size as 2
//   // ExtendibleHash<int, int> *test =
//   //         new ExtendibleHash<int, int>(10);
//   auto test = std::make_unique<ExtendibleHashTable<int, int>>(10);
//   for (int i=0; i<10; i++) {
//     test->Insert(i, i);
//   }

//   for (int i=0; i<10; i++) {
//     srand(time(0)+i);
//     if (rand()%2==0) {
//       test->Remove(i);
//       int value;
//       EXPECT_NE(test->Find(i, value), true);
//     } else {
//       test->Insert(i, i+2);
//       int value;
//       EXPECT_EQ(test->Find(i, value), true);
//       EXPECT_EQ(value, i+2);
//     }
//   }
// }
// #define TEST_NUM 1000
// #define BUCKET_SIZE 64
// TEST(ExtendibleHashTest, BasicRandomTest) {
//   // ExtendibleHash<int, int> *test = new ExtendibleHash<int, int>();
//   auto test = std::make_unique<ExtendibleHashTable<int, int>>(2);
//   // insert
//   int seed = time(nullptr);
//   std::cerr << "seed: " << seed << std::endl;
//   std::default_random_engine engine(seed);
//   std::uniform_int_distribution<int> distribution(0, TEST_NUM);
//   std::map<int, int> comparator;

//   for (int i = 0; i < TEST_NUM; ++i) {
//     std::cerr << i << std::endl;
//     auto item = distribution(engine);
//     comparator[item] = item;
//     printf("%d,",item);
//     test->Insert(item, item);
//     //std::cerr << std::dec << item << std::hex << "( 0x" << item << " )" << std::endl;
//   }
//   //printf("\n");

//   // compare result
//   int value = 0;
//   for (auto i: comparator) {
//     test->Find(i.first, value);
//     //printf("%d,%d\n",,i.first);
//     EXPECT_EQ(i.first, value);
//     // delete
//     EXPECT_EQ(1, test->Remove(value));
//     // find again will fail
//     value = 0;
//     EXPECT_EQ(0, test->Find(i.first, value));
//   }
// }
}  // namespace bustub

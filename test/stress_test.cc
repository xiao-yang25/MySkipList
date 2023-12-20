#include <iostream>
#include <chrono> 
// #include <cstdlib>
#include <thread>
#include <vector>
#include <random>
#include <functional>
#include "../src/skip_list.h"

namespace {
  constexpr int NUM_THREADS = 1;
  constexpr int TEST_COUNT = 100000;
  skip_list::SkipList<int, std::string> test_skip_list(18);
}

int GetRandomNumber(std::mt19937 &gen) {
  std::uniform_int_distribution<int> distrib(0, 99);
  return distrib(gen);
}

void TestInsertElement(int tid, std::mt19937 &gen) {
  std::cout << tid << std::endl;  
  int count_each_thread = TEST_COUNT/NUM_THREADS; 
  for (int i = tid*count_each_thread, count = 0; count < count_each_thread; i++) {
    count++;
	  test_skip_list.InsertElement(GetRandomNumber(gen) % TEST_COUNT, "rain"); 
  }
}

void TestGetElement(int tid, std::mt19937 &gen) {
  std::cout << tid << std::endl;  
  int count_each_thread = TEST_COUNT/NUM_THREADS; 
	for (int i = tid*count_each_thread, count = 0; count < count_each_thread; i++) {
    count++;
		test_skip_list.SearchElement(GetRandomNumber(gen) % TEST_COUNT); 
	}
}

int main() {
  unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::mt19937 gen(seed);

  {
    std::vector<std::thread> threads(NUM_THREADS);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; i++ ) {
      std::cout << "main() : creating thread, " << i << std::endl;
      threads.emplace_back(TestInsertElement, i, std::ref(gen));
    }

    for(auto& thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }

    auto finish = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    std::cout << "test insert element end."  << std::endl;
  }

  //test_skip_list.PrintList();

  // {
  //   std::vector<std::thread> threads(NUM_THREADS);
  //   auto start = std::chrono::high_resolution_clock::now();

  //   for(int i = 0; i < NUM_THREADS; i++ ) {
  //     std::cout << "main() : creating thread, " << i << std::endl;
  //     threads.emplace_back(TestGetElement, i, std::ref(gen));
  //   }

  //   for(auto& thread : threads) {
  //     if (thread.joinable()) {
  //       thread.join();
  //     }
  //   }

  //   auto finish = std::chrono::high_resolution_clock::now(); 
  //   std::chrono::duration<double> elapsed = finish - start;
  //   std::cout << "get elapsed:" << elapsed.count() << std::endl;
  //   std::cout << "test find element end."  << std::endl;
  // }

  return 0;
}

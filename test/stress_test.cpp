#include <iostream>
#include <chrono> 
#include <cstdlib>
#include <pthread.h> 
#include <time.h>
#include <thread>
#include <vector>
#include "src/skip_list.h"

namespace {
  constexpr int NUM_THREADS = 1;
  constexpr int TEST_COUNT = 100000;
  skip_list::SkipList<int, std::string> test_skip_list(18);
}

void *TestInsertElement(void* threadid) {
  long tid; 
  tid = (long)threadid;
  std::cout << tid << std::endl;  
  int tmp = TEST_COUNT/NUM_THREADS; 
  for (int i=tid*tmp, count=0; count<tmp; i++) {
    count++;
	test_skip_list.InsertElement(rand() % TEST_COUNT, "rain"); 
  }
  pthread_exit(NULL);
}

void *TestGetElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		test_skip_list.SearchElement(rand() % TEST_COUNT); 
	}
    pthread_exit(NULL);
}

int main() {
  srand (time(NULL));  

  {
    pthread_t threads[NUM_THREADS];
    int rc;
    int i;

    //高精度时间
    auto start = std::chrono::high_resolution_clock::now();

    for ( i = 0; i < NUM_THREADS; i++ ) {

        std::cout << "main() : creating thread, " << i << std::endl;

        //创建线程
        rc = pthread_create(&threads[i], NULL, insertElement, (void *)i);

        if (rc) {
            std::cout << "Error:unable to create thread," << rc << std::endl;
            exit(-1);
        }
    }

    void *ret;
    for ( i = 0; i < NUM_THREADS; i++ ) {

        //等待线程结束
        if (pthread_join(threads[i], &ret) !=0 )  {
            perror("pthread_create() error"); 
            exit(3);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now(); 

    //响应时间
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "insert elapsed:" << elapsed.count() << std::endl;
  }

  test_skip_list.PrintList();

//   {
//     std::vector<std::thread> threads(NUM_THREADS);
//     int rc;
//     int i;
//     auto start = std::chrono::high_resolution_clock::now();

//     for( i = 0; i < NUM_THREADS; i++ ) {
//         std::cout << "main() : creating thread, " << i << std::endl;
//         threads.emplace_back(TestGetElement, i);
//     }

//     for(auto& thread : threads) {
//         if (thread.joinable()) {
//             thread.join();
//         }
//     }

//     auto finish = std::chrono::high_resolution_clock::now(); 
//     std::chrono::duration<double> elapsed = finish - start;
//     std::cout << "get elapsed:" << elapsed.count() << std::endl;
//   }

  return 0;
}

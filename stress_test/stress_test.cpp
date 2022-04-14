#include <iostream>
#include <chrono> //c++11 时间库
#include <cstdlib>
#include <pthread.h> //提供线程相关函数
#include <time.h>
#include "../my_skip_list.h"

#define NUM_THREADS 1
#define TEST_COUNT 100000
SkipList<int, std::string> skipList(18);

void *insertElement(void* threadid) {
    
    //线程id
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  

    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.insert_element(rand() % TEST_COUNT, "rain"); 
	}
    pthread_exit(NULL);
}

void *getElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.search_element(rand() % TEST_COUNT); 
	}
    pthread_exit(NULL);
}

int main() {
    srand (time(NULL));  
    // {

    //     pthread_t threads[NUM_THREADS];
    //     int rc;
    //     int i;

    //     //高精度时间
    //     auto start = std::chrono::high_resolution_clock::now();

    //     for ( i = 0; i < NUM_THREADS; i++ ) {

    //         std::cout << "main() : creating thread, " << i << std::endl;

    //         //创建线程
    //         rc = pthread_create(&threads[i], NULL, insertElement, (void *)i);

    //         if (rc) {
    //             std::cout << "Error:unable to create thread," << rc << std::endl;
    //             exit(-1);
    //         }
    //     }

    //     void *ret;
    //     for ( i = 0; i < NUM_THREADS; i++ ) {

    //         //等待线程结束
    //         if (pthread_join(threads[i], &ret) !=0 )  {
    //             perror("pthread_create() error"); 
    //             exit(3);
    //         }
    //     }
    //     auto finish = std::chrono::high_resolution_clock::now(); 

    //     //响应时间
    //     std::chrono::duration<double> elapsed = finish - start;
    //     std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    // }

     //skipList.displayList();

    {
        pthread_t threads[NUM_THREADS];
        int rc;
        int i;
        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, getElement, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error"); 
                exit(3);
            }
        }

        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "get elapsed:" << elapsed.count() << std::endl;
    }


    //终止线程
	pthread_exit(NULL);
    return 0;
}

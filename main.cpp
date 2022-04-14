#include<iostream>
#include "my_skip_list.h"
#define FILE_PATH "./store/dumpFile"

int main() {

    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    SkipList<int, std::string> skipList(6);

	skipList.insert_element(1, "是徒为静养"); 
	skipList.insert_element(3, "而不用克己工夫也"); 
	skipList.insert_element(7, "如此"); 
	skipList.insert_element(8, "临事便要倾倒"); 
	skipList.insert_element(9, "人需在事上磨"); 
	skipList.insert_element(19, "方可立得住"); 
	skipList.insert_element(19, "方能静亦定,动亦定"); 

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search_element(9);
    skipList.search_element(18);


    skipList.print_list();

    skipList.delete_element(3);
    skipList.delete_element(7);

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.print_list();

}
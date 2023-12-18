#include<iostream>
#include "skip_list.h"
#define FILE_PATH "config/dumpFile"

int main() {
  skip_list::SkipList<int, std::string> skip_list(6);

  skip_list.InsertElement(1, "是徒为静养"); 
  skip_list.InsertElement(3, "而不用克己工夫也"); 
  skip_list.InsertElement(7, "如此"); 
  skip_list.InsertElement(8, "临事便要倾倒"); 
  skip_list.InsertElement(9, "人需在事上磨"); 
  skip_list.InsertElement(19, "方可立得住"); 
  skip_list.InsertElement(19, "方能静亦定,动亦定"); 

  std::cout << "skip_list size:" << skip_list.Size() << std::endl;

  skip_list.DumpFile();

  // skip_list.load_file();

  skip_list.SearchElement(9);
  skip_list.SearchElement(18);

  skip_list.PrintList();

  skip_list.DeleteElement(3);
  skip_list.DeleteElement(7);

  std::cout << "skip_list size:" << skip_list.Size() << std::endl;

  skip_list.PrintList();
}

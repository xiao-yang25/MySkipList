/* ************************************************************************

> File Name:     my_skip_list.h
> Author:        xiao-yang25
> Created Time:  Thur Apr 13 2021
> Description:   This is a lightweight key-value storage engine based on skip list.

 ************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <mutex> 
#include <vector>
#include <memory>

#define STORE_FILE "config/dumpFile"  //转存的文件名

template<typename K, typename V>
using NodeVec = std::vector<std::shared_ptr<Node<K, V>>>;
namespace node {
//define K-V node
template<typename K, typename V>
class Node {
 public:
  Node() {}
  Node(K k, V v, int);
  ~Node();
  K GetKey() const;
  V GetValue() const;
  void SetValue(V);

  // use to store next Node for each level
  NodeVec forward_;
  //level of specific Node 
  int node_level_;

 private:
  K key_;
  V value_;
};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) 
  : key_(k), value_(v), node_level_(level) {};

template<typename K, typename V>
Node<K, V>::~Node() {
    
};

template<typename K, typename V>
K Node<K, V>::GetKey() const {
    return key_;
};

template<typename K, typename V>
V Node<K, V>::GetValue() const {
    return value_;
};

template<typename K, typename V>
void Node<K, V>::SetValue(V value) {
    value_ = value;
};
} //  namespace node

namespace skip_list {
const std::string delimiter = ":";
//define skip_list
template<typename K, typename V>
class SkipList {
 public:
  SkipList(int);
  ~SkipList();
  int GetRandomLevel();
  std::shared_ptr<node::Node<K, V>> CreateNode(K, V, int);
  int InsertElement(K, V);
  void PrintList();
  bool SearchElement(K);
  void DeleteElement(K);
  void DumpFile();
  void LoadFile();
  int Size();

 private:
  void GetKeyValueFromString(const std::string& str, std::string* key, std::string* value);
  bool IsValidString(const std::string& str);

 private:
  int max_level_;
  int current_level_;
  std::shared_ptr<node::Node<K, V>> header_;
  std::ofstream file_writer_;
  std::ifstream file_reader_;
  int element_count_;
  std::mutex mutex_;
};

template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level) 
  : max_level_(max_level),
    current_level_(0),
    element_count_(0) {
  header_ = std::make_shared<node::Node<K, V>>(K(), V(), max_level);
};

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
  if (file_writer_.is_open()) {
    file_writer_.close();
  }
  if (file_reader_.is_open()) {
     file_reader_.close();
  }
}

template<typename K, typename V>
int SkipList<K, V>::GetRandomLevel() {
  int level = 1;
  while (rand() % 2) {
    level++;
  }
  level = (level < max_level_) ? level : max_level_;
  return level;
};

template<typename K, typename V>
std::shared_ptr<node::Node<K, V>> SkipList<K, V>::CreateNode(const K k, const V v, int level) {
  auto node = std::make_shared<node::Node<K, V>>(k, v, level);
  return node;
}

//return 1 means element exits, 0 means insert sucessfully
template<typename K, typename V>
int SkipList<K, V>::InsertElement(const K key, const V value) {
  std::unique_lock<std::mutex> lck(mutex_);
  auto current = header_;

  //store last node ptr less than key for each level
  auto update = NodeVec<K, V>(max_level_ + 1);
    
  for (int i = current_level_; i >= 0; i--) {
    while (current->forward_[i] != nullptr && current->forward_[i]->GetKey() < key) {
      current = current->forward_[i];
    }
    //store node ptr when jump to next level
    update[i] = current;
  }

  //now reach 0 level, next node is where to insert
  current = current->forward_[0];
  //if node exits, unlock then return
  if (current != nullptr && current->GetKey() == key) {
    std::cout << "key: " << key << ",exists" << std::endl;
    lck.unlock();
    return 1;
  }
  //如果current为空则说明此时在 0 level
  //不为空则在update[0]和current之间插入
  if (current == nullptr || current->GetKey() != key) {
    int random_level = GetRandomLevel();

    //if random_level > current_level, level + 1, and update the update vec
    if (random_level > current_level_) {
      for (int i = current_level_ + 1; i < random_level + 1; i++) {
          update[i] = header_;
      }
      current_level_ = random_level;
    }

    auto inserted_node = CreateNode(key, value, random_level);

    //insert node
    for (int i = 0; i <= random_level; i++) {
        inserted_node->forward_[i] = update[i]->forward_[i];
        update[i]->forward_[i] = inserted_node;
    }
    std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
    element_count_++;
  }
  lck.unlock();
  return 0;
}

template<typename K, typename V>
void SkipList<K, V>::PrintList() {
  std::cout << "\n*****Skip List*****"<<"\n";
  for (int i = 0; i <= current_level_; i++) {
    auto node = header_->forward_[i];
    std::cout << "Level " << i << ": ";
    while (node != nullptr) {
      std::cout << node->GetKey() << ":" << node->GetValue() << ";";
      node = node->forward[i];
    }
    std::cout << std::endl;
  }
}

template<typename K, typename V>
void SkipList<K, V>::DumpFile() {
  std::cout << "dump_file-----------------" << std::endl;
  file_writer_.open(STORE_FILE);
  auto node = header_->forward[0];
  while (node != nullptr) {
    file_writer_ << node->GetKey() << ":" << node->GetValue() << "\n";
    std::cout << node->GetKey() << ":" << node->GetValue() << ";\n";
    node = node->forward[0];
  }

  file_writer_.flush();
  file_writer_.close();
  return ;
}

template<typename K, typename V>
void SkipList<K, V>::LoadFile() {
  file_reader_.open(STORE_FILE);
  std::cout << "load_file-----------------" << std::endl;
  std::string line;
  std::string* key = new std::string();
  std::string* value = new std::string();
  while (getline(file_reader_, line)) {
    GetKeyValueFromString(line, key ,value);
    if (key->empty() || value->empty()) {
      continue;
    }
    InsertElement(*key, *value);
    std::cout << "key:" << *key << "value:" << *value << std::endl;
  }
  file_reader_.close();
}

template<typename K, typename V>
int SkipList<K, V>::Size() {
  return element_count_;
}

template<typename K, typename V>
void SkipList<K, V>::GetKeyValueFromString(const std::string& str, std::string* key, std::string* value) {
  if (!IsValidString(str)) {
    return;
  }
  //如key:value ，分别获取冒号前后的部分
  *key = str.substr(0, str.find(delimiter));
  *value = str.substr(str.find(delimiter)+1, str.length());
}

//判断字符串是否合法
template<typename K, typename V>
bool SkipList<K, V>::IsValidString(const std::string& str) {
  if (str.empty()) {
    return false;
  }
  //如果没有找到 ： ,则说明该字符串不合法（npos通常用来表示没有找到要查找的对象）
  if (str.find(delimiter) == std::string::npos) {
    return false;
  }
  return true;
}

template<typename K, typename V>
void SkipList<K, V>::DeleteElement(K key) {
  std::unique_lock<std::mutex> lck(mutex_);
  auto current = header_;
  auto update = NodeVec<K, V>(max_level_ + 1);
  for (int i = current_level_; i >= 0; i--) {
    while (current->forward[i] != nullptr && current->forward[i]->GetKey() < key) {
      current = current->forward[i];
    }
    update[i] = current;
  }

  current = current->forward[0];
  if (current != nullptr && current->get_key() == key) {
    for (int i = 0; i <= current_level_; i++) {
      if (update[i]->forward[i] != current) break;
      update[i]->forward[i] = current->forward[i];
    }

    while (current_level_ > 0 && header_->forward[current_level_] == 0) {
        current_level_--;
    }

    std::cout << "Successfully deleted key "<< key << std::endl;
    element_count_ --;
    //delete current;
  }
  lck.unlock();
  return;
}

//搜索结点
template<typename K, typename V>
bool SkipList<K, V>::SearchElement(K key) {
  std::cout << "search_element-----------------" << std::endl;
  auto current = header_;
  //从最高层开始找
  for (int i = current_level_; i >= 0; i--) {
    while (current->forward_[i] != nullptr && current->forward_[i]->GetKey() < key) {
      current = current->forward_[i];
    }
  }
  current = current->forward_[0];
  //若相等则说明找到
  if (current && current->GetKey() == key) {
    std::cout << "Found key: " << key << ", value: " << current->GetValue() << std::endl;
    return true;
  }

  std::cout << "Not Found Key:" << key << std::endl;
  return false;
}

}  //  namespace skiplist

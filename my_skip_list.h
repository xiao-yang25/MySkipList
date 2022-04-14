/* ************************************************************************

> File Name:     my_skip_list.h
> Author:        xiao-yang25
> Created Time:  Thur Apr 13 2021
> Description:   This is a lightweight key-value storage engine based on skip list.

 ************************************************************************/

#include<iostream>
#include<cstring>
#include<cstdlib>
#include<cmath>
#include<fstream>
#include<mutex> //要用到其中的mutex(普通的互斥锁）类型

#define STORE_FILE "store/dumpFile"  //转存的文件名

std::mutex mtx;
std::string delimiter = ":";

//定义K-V结点
template<typename K, typename V>
class Node {

public:

    Node() {}

    Node(K k, V v, int);
    
    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);

    //用于存放每一层下一个结点的指针，可写成*forward[_max_level]
    Node<K, V> **forward;

    //该结点的层数
    int node_level;

private:
    K key;
    V value;
};

//构造函数
template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) {
    
    this->key = k;
    this->value = v;
    this->node_level = level;

    //数组下标从0开始，所以+1;
    this->forward = new Node<K, V>*[level+1];

    //初始化为0
    memset(this->forward, 0, sizeof(Node<K, V>*)*(level+1));
};

//析构函数
template<typename K, typename V>
Node<K, V>::~Node() {
    //只删除forward可能会造成内存泄露
    delete []forward;
};

//获取key
template<typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
};

//获取value
template<typename K, typename V>
V Node<K, V>::get_value() const {
    return value;
};

//设置value
template<typename K, typename V>
void Node<K, V>::set_value(V value) {
    this->value = value;
};

//定义跳表
template<typename K, typename V>
class SkipList {

public:

    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void print_list();
    bool search_element(K);
    void delete_element(K);

    //防止数据丢失及其他情况，转存入文件
    void dump_file();
    void load_file();

    int size();

private:

    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

private:
    //跳表最大level
    int _max_level;

    //跳表当前level
    int _skip_list_level;

    //指向头结点的指针
    Node<K, V> *_header;

    //文件操作 写和读
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    //当前元素个数
    int _element_count;
};

//创建新结点
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
 
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}

//插入元素
//返回 1 表示元素存在
//返回 0 表示插入成功
template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {
    
    mtx.lock();

    //创建并将头指针赋予当前指针
    Node<K, V> *current = this->_header;

    //创建一个数组update，用于存储第i层最后一个比要插入的key小的结点的指针你
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    //从最高层开始遍历
    for (int i = _skip_list_level; i>=0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;//要跳到下一层的时候存储此时的结点指针
    }

    //遍历完了之后达到了0层，此时再往下一个结点就是要插入的位置
    current = current->forward[0];

    //如果存在此结点则解锁后返回
    if (current != nullptr && current->get_key() == key) {
        std::cout << "key: " << key << ",exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    //如果current为空则说明此时在 0 level
    //不为空则在update[0]和current之间插入
    if (current == nullptr || current->get_key() != key) {

        // 产生随机层数
        int random_level = get_random_level();

        //如果随机出来的层数大于当前层数，则从当前层往上开辟新的层数，并将update更新为头结点指针
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level+1; i < random_level+1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        Node<K, V>* inserted_node = create_node(key, value, random_level);

        //插入结点，类似链表插入操作
        for (int i=0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count ++;
    }
    mtx.unlock();
    return 0;
}

//打印跳表
template<typename K, typename V>
void SkipList<K, V>::print_list() {

    std::cout << "\n*****Skip List*****"<<"\n";
    for (int i=0; i <= _skip_list_level; i++) {
        Node<K, V> *node = this->_header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != nullptr) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

//转存
template<typename K, typename V>
void SkipList<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K, V> *node = this->_header->forward[0];

    while (node != nullptr) {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    //清空缓存区
    _file_writer.flush();

    _file_writer.close();
    return ;
}

//载入
template<typename K, typename V>
void SkipList<K, V>::load_file() {

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();

    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key ,value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    _file_reader.close();
}

//返回结点个数
template<typename K, typename V>
int SkipList<K, V>::size() {
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {

    if (!is_valid_string(str)) {
        return;
    }
    //如key:value ，分别获取冒号前后的部分
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

//判断字符串是否合法
template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    
    //如果没有找到 ： ,则说明该字符串不合法（npos通常用来表示没有找到要查找的对象）
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

//删除结点,参考插入结点
template<typename K, typename V>
void SkipList<K, V>::delete_element(K key) {

    mtx.lock();
    Node<K, V> *current = this->_header;
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    for (int i=_skip_list_level; i>=0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != nullptr && current->get_key() == key) {
        //从最低层开始删除
        for (int i = 0; i <= _skip_list_level; i++) {
            //如果下一个结点不是目标节点，跳出
            if (update[i]->forward[i] != current)
                break;
            //  forward越过要删除的节点指向下一个
            update[i]->forward[i] = current->forward[i];
        }

        //如果删除结点后该层无元素，则调整当前层数
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {
            _skip_list_level --;
        }

        std::cout << "Successfully deleted key "<< key << std::endl;
        _element_count --;
        //delete current;
    }
    mtx.unlock();
    return;
}

//搜索结点
template<typename K, typename V>
bool SkipList<K, V>::search_element(K key) {

    std::cout << "search_element-----------------" << std::endl;
    Node<K, V> *current = _header;

    //从最高层开始找
    for (int i=_skip_list_level; i>=0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];

    //若相等则说明找到
    if (current && current->get_key() == key) {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

//跳表构造函数
template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level) {

    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    //创建头结点，并初始化key和value的值
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
};

//跳表析构函数
template<typename K, typename V>
SkipList<K, V>::~SkipList() {

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }

    //只删除头结点可能存在内存泄露
    delete _header;

}

//获取随机层数
template<typename K, typename V>
int SkipList<K, V>::get_random_level() {

    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return  k;
};



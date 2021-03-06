# 基于跳表SkipList实现的轻量级KV存储引擎

使用语言主要为C++，包含数据的插入、删除、查询、打印、转储、文件加载等操作。用普通台式机测试的数据为：每秒可处理写请求数（QPS）: 16.44w，每秒可处理读请求数（QPS）: 19.23w

# 存储引擎数据表现

## 插入操作

跳表树高：18 

在普通台式机上测试的结果为：


|插入数据规模（万条） |耗时（秒） | 
|---|---|
|10 |0.45244 |
|50 |2.76927 |
|100 |6.08354 |

每秒可处理写请求数（QPS）: 16.44w

## 取数据操作

|取数据规模（万条） |耗时（秒） | 
|---|---|
|10|0.51340 |
|50|2.41404 |
|100|5.19967 |

每秒可处理读请求数（QPS）: 19.23w

# 运行和测试

编译运行：

```
make            // 编译
./bin/main      // 运行
```

性能测试：

```
sh stress_test_start.sh 
```


# 主要文件和接口

## 主要文件

* main.cpp 主函数数据操作
* skiplist.h 跳表核心实现       
* bin 生成可执行文件目录 
* makefile 编译脚本
* store 数据落盘文件夹 
* stress_test_start.sh 压力测试脚本

## 主要接口

* insertElement（插入）
* deleteElement（删除）
* searchElement（查询）
* printList（打印）
* dumpFile（转储）
* loadFile（加载）

# 参考资料

https://github.com/youngyangyang04/Skiplist-CPP.git

https://blog.csdn.net/wyn1564464568/article/details/121194460

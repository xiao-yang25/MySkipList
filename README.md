# 基于C++11的轻量级KV存储引擎

这个项目是基于去年我转行学C++的第一个项目，参考自程序员Carl的https://github.com/youngyangyang04/Skiplist-CPP
当时是基本仿照原项目，现在在某中厂工作半年了，用C++11重构了这个项目，加入一些C++11的新特性，比如：

* 列表初始化
* 别名模板
* 智能指针 
* 互斥锁
* constexpr 变量
* 多线程
  
项目包含数据的插入、删除、查询、打印、转储、文件加载等操作。用笔记本测试的数据为：每秒可处理写请求数（QPS）: 26.75w，每秒可处理读请求数（QPS）: 27.62w

# 存储引擎数据表现

## 插入操作

跳表树高：18 

在笔记本上测试的结果为：


|插入数据规模（万条） |耗时（秒） | 
|---|---|
|10 |0.36815 |
|50 |1.81781 |
|100 |3.73812 |

每秒可处理写请求数（QPS）: 26.75w

## 取数据操作

|取数据规模（万条） |耗时（秒） | 
|---|---|
|10|0.28066 |
|50|1.59423 |
|100|3.62017 |

每秒可处理读请求数（QPS）: 27.62w

# 运行和测试

编译运行：

```
./build.sh            // 编译
./run.sh      // 运行main函数
./run.sh -t   // 性能测试
```

# 主要接口

## 主要接口

* InsertElement（插入）
* DeleteElement（删除）
* SearchElement（查询）
* PrintList（打印）
* DumpFile（转储）
* LoadFile（加载）

# 参考资料

https://github.com/youngyangyang04/Skiplist-CPP.git

https://blog.csdn.net/wyn1564464568/article/details/121194460

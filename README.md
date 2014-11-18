# Naesala

A Common C++ Library for Linux


## Features

BlockingQueue

* 分别用 条件变量（condition）、信号量（semaphore） 实现的简单阻塞队列

* 提供 Linux、std C++ 11、Boost 三个版本

Common

* Barrier：内存栅栏

* ProcStat：统计Linux进程信息

* Range：实现类似python range的功能

Configure

* ConfParser：解析ini格式的配置文件

Hashmap

* LinearHashmap：高性能线性hash表

Lockfree

* LockfreeStack：无锁栈，实现自 Treiber 算法

* LockfreeList：无锁队列，实现自 Michael Scott 算法

## Benckmark

BlockingQueue

* 条件变量和信号量实现的阻塞队列的性能比较

Hashmap

* C++/Gnu/Boost/第三方开源/LinearHashmap hashmap性能比较

* 16,777,216 operations

> | Hashmap | Insert | Traverse | Find | Delete |
> | :-------- | :--------| :--------| :--------| :--------|
> | [std::map] | 15.3459s | 1.02469s | 17.642s | 2.027s |
> | [__gnu_cxx::hash_map] | 4.07776s | 1.062s | 4.17001s | 0.33801s |
> | [std::unordered_map] | 5.132s | 0.384036s | 2.53397s | 0.569996s |
> | [std::tr1::unordered_map] | 3.94881s | 0.303182s | 1.924s | 0.334859s |
> | [boost::unordered_map] | 4.997s | 0.442003s | 3.85299s | 0.574989s |
> | [align_hash_map] | 2.00799s | 0.0600757s | 1.04993s | 0.0911587s |
> | [LinearHashmap] | 0.914718s | 0.0471907s | 0.746924s | 0.0620157s |

Lockfree

* LockfreeStack、LockfreeList功能验证


## Dependings

* cmake
* boost-devel


================================
by Xiaojie Chen (swly@live.com)


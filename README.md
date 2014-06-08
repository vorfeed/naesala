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

Lockfree

* LockfreeStack：无锁栈，实现自 Treiber 算法

* LockfreeList：无锁队列，实现自 Michael Scott 算法

## Benckmark

BlockingQueue

* 条件变量和信号量实现的阻塞队列的性能比较

Hashmap

* C++/Gnu/Boost/第三方开源 hashmap性能比较

> | Hashmap | Insert | Traverse | Find | Delete |
> | :-------- | :--------| :--------| :--------| :--------|
> | [std::map]  | 1.33993s | 0.089017s | 1.24957s | 2.63042s |
> | [__gnu_cxx::hash_map] | 0.42164s | 0.113012s | 0.238216s | 0.404426s |
> | [std::unordered_map] | 0.527232s | 0.042736s | 0.530173s | 0.60149s |
> | [std::tr1::unordered_map] | 0.35997s | 0.075255s | 0.390161s | 0.371356s |
> | [boost::unordered_map] | 0.651387s | 0.036531s | 0.692967s | 0.785001s |
> | [align_hash_map] | 0.159173s | 0.041413s | 0.169958s | 0.150414s |

Lockfree

* LockfreeStack、LockfreeList功能验证


## Dependings

* cmake
* boost-devel


================================
by Xiaojie Chen (swly@live.com)


# Naesala

A Common C++ Library for Linux


## Features

BlockingQueue

* �ֱ��� ����������condition�����ź�����semaphore�� ʵ�ֵļ���������

* �ṩ Linux��std C++ 11��Boost �����汾

Common

* Barrier���ڴ�դ��

* ProcStat��ͳ��Linux������Ϣ

* Range��ʵ������python range�Ĺ���

Configure

* ConfParser������ini��ʽ�������ļ�

Lockfree

* LockfreeStack������ջ��ʵ���� Treiber �㷨

* LockfreeList���������У�ʵ���� Michael Scott �㷨

## Benckmark

BlockingQueue

* �����������ź���ʵ�ֵ��������е����ܱȽ�

Hashmap

* C++/Gnu/Boost/��������Դ hashmap���ܱȽ�

> | Hashmap | Insert | Traverse | Find | Delete |
> | :-------- | :--------| :--------| :--------| :--------|
> | [std::map]  | 1.33993s | 0.089017s | 1.24957s | 2.63042s |
> | [__gnu_cxx::hash_map] | 0.42164s | 0.113012s | 0.238216s | 0.404426s |
> | [std::unordered_map] | 0.527232s | 0.042736s | 0.530173s | 0.60149s |
> | [std::tr1::unordered_map] | 0.35997s | 0.075255s | 0.390161s | 0.371356s |
> | [boost::unordered_map] | 0.651387s | 0.036531s | 0.692967s | 0.785001s |
> | [align_hash_map] | 0.159173s | 0.041413s | 0.169958s | 0.150414s |

Lockfree

* LockfreeStack��LockfreeList������֤


## Dependings

* cmake
* boost-devel


================================
by Xiaojie Chen (swly@live.com)


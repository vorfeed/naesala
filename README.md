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

Hashmap

* LinearHashmap������������hash��

Lockfree

* LockfreeStack������ջ��ʵ���� Treiber �㷨

* LockfreeList���������У�ʵ���� Michael Scott �㷨

## Benckmark

BlockingQueue

* �����������ź���ʵ�ֵ��������е����ܱȽ�

Hashmap

* C++/Gnu/Boost/��������Դ/LinearHashmap hashmap���ܱȽ�

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

* LockfreeStack��LockfreeList������֤


## Dependings

* cmake
* boost-devel


================================
by Xiaojie Chen (swly@live.com)


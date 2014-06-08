// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <iostream>

#include <naesala/common/range.h>
#include <naesala/common/proc_stat.h>

using namespace naesala;

int main(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i : range(argc - 1)) {
            ProcStat ps(atoi(argv[i + 1]));
            std::cout << ps << std::endl;
        }
    } else {
        ProcStat ps;
        std::cout << ps << std::endl;
    }

    return 0;
}

/*

[root@localhost build]# ./procstat  4249
---------------------------------------------
| pid             |                    4249 |
| command         |             dbus-daemon |
| task_state      |                       S |
| euid            |                       0 |
| egid            |                       0 |
| ppid            |                       1 |
| pgid            |                    4249 |
| sid             |                    4249 |
| tty_nr          |                       0 |
| tty_pgrp        |                      -1 |
| task_flags      |                 4202816 |
| min_flt         |                     235 |
| cmin_flt        |                       0 |
| maj_flt         |                       0 |
| cmaj_flt        |                       0 |
| utime           |                       0 |
| stime           |                       0 |
| cutime          |                       0 |
| cstime          |                       0 |
| priority        |                      18 |
| nice            |                       0 |
| num_threads     |                       1 |
| it_real_value   |                       0 |
| start_time      |                    5328 |
| vsize           |                35770368 |
| rss             |                     311 |
| rlim            |    18446744073709551615 |
| start_code      |          47260140863488 |
| end_code        |          47260141160596 |
| start_stack     |         140736905063376 |
| kstkesp         |    18446744073709551615 |
| kstkeip         |          47260157274751 |
| pendingsig      |                       0 |
| block_sig       |                       0 |
| sigign          |                       0 |
| sigcatch        |               268451841 |
| wchan           |     3335634162290753796 |
| nswap           |                       0 |
| cnswap          |                       0 |
| exit_signal     |                      17 |
| task_cpu        |                       1 |
| task_rt_priority|                       0 |
| task_policy     |                       0 |
---------------------------------------------

*/

// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

namespace naesala {

/// the structure corresponding the content in /proc/<pid>/stat
struct ProcStat {
    int           pid;                      // The process id.
    char          command[_POSIX_PATH_MAX]; // The filename of the executable
    char          task_state;   // 1        // R is running, S is sleeping,
                                            // D is sleeping in an uninterruptible wait,
                                            // Z is zombie, T is traced or stopped
    int           ppid;                     // The pid of the parent.
    int           pgid;                     // The pgid of the process.
    int           sid;                      // The sid id of the process.
    int           tty_nr;                   // The tty_nr the process uses
    int           tty_pgrp;                 // (too long)
    unsigned int  task_flags;               // The task_flags of the process.
    unsigned int  min_flt;                  // The number of minor faults
    unsigned int  cmin_flt;                 // The number of minor faults with childs
    unsigned int  maj_flt;                  // The number of major faults
    unsigned int  cmaj_flt;                 // The number of major faults with childs
    int           utime;                    // user mode jiffies
    int           stime;                    // kernel mode jiffies
    int           cutime;                   // user mode jiffies with childs
    int           cstime;                   // kernel mode jiffies with childs
    int           priority;                 // process's next timeslice
    int           nice;                     // the standard nice value, plus fifteen
    unsigned int  num_threads;              // The time in jiffies of the next num_threads
    unsigned int  it_real_value;            // The time before the next SIGALRM is sent to the process
    int           start_time;  // 20        // Time the process started after system boot
    unsigned long long  vsize;              // Virtual memory size
    unsigned long long  rss;                // Resident Set Size
    unsigned long long  rlim;               // Current limit in bytes on the rss
    unsigned long long  start_code;         // The address above which program text can run
    unsigned long long  end_code;           // The address below which program text can run
    unsigned long long  start_stack;        // The address of the start of the stack
    unsigned long long  kstkesp;            // The current value of ESP
    unsigned long long  kstkeip;            // The current value of EIP
    int           pendingsig;               // The bitmap of pending pendingsigs
    int           block_sig;  // 30         // The bitmap of block_sig pendingsigs
    int           sigign;                   // The bitmap of ignored pendingsigs
    int           sigcatch;                 // The bitmap of catched pendingsigs
    unsigned long long  wchan;  // 33       // (too long)
    unsigned long long  nswap;              // The num of swapped pages
    int           cnswap;                   // The num of swapped pages in all subprocess
    int           exit_signal;              // The signal to parent process when current process ends
    int           task_cpu;                 // Which cpu current task run in
    int           task_rt_priority;         // Relative priority of real-time processes
    int           task_policy;              // The scheduling strategy of process
                                            // 0 is Non real-time process
                                            // 1 is FIFO real-time process
                                            // 2 is RR real-time process

    unsigned int  euid;                     // effective user id
    unsigned int  egid;                     // effective group id

public:
    ProcStat() { update(); }
    ProcStat(pid_t pid) { update(pid); }

    /// Get statistics of the process according to pid
    bool update(pid_t pid) {
        std::string statfile("/proc/" + std::to_string((unsigned)pid) + "/stat");

        if (-1 == access(statfile.c_str(), R_OK)) {
            return false;
        }

        struct stat st;
        if (-1 != stat(statfile.c_str(), &st)) {
            euid = st.st_uid;
            egid = st.st_gid;
        } else {
            euid = egid = (unsigned int) -1;
        }

        std::ifstream in(statfile);
        if (!in.is_open()) {
            return false;
        }

        std::string temp;
        in >> this->pid;
        in >> temp;
        memcpy(command, temp.substr(1, temp.length() - 2).c_str(), temp.length() - 1);

        in >> task_state >> ppid >> pgid >> sid >> tty_nr
            >> tty_pgrp >> task_flags >> min_flt >> cmin_flt
            >> maj_flt >> cmaj_flt >> utime >> stime
            >> cutime >> cstime >> priority >> nice
            >> num_threads >> it_real_value >> start_time >> vsize
            >> rss >> rlim >> start_code >> end_code
            >> start_stack >> kstkesp >> kstkeip >> pendingsig
            >> block_sig >> sigign >> sigcatch >> wchan
            >> nswap >> cnswap >> exit_signal >> task_cpu
            >> task_rt_priority >> task_policy;

        return true;
    }

    /// Get statistics of the process where this function is called
    bool update() {
        return update(getpid());
    }

    friend std::ostream& operator<<(std::ostream& out, ProcStat const& stat) {
#define set_value(value) \
    out << "| " << std::left << std::setw(16) << #value << "|" << std::right << std::setw(24) << stat.value << " |" << std::endl

        std::string division(45, '-');
        out << division << std::endl;
        set_value(pid);
        set_value(command);
        set_value(task_state);
        set_value(euid);
        set_value(egid);
        set_value(ppid);
        set_value(pgid);
        set_value(sid);
        set_value(tty_nr);
        set_value(tty_pgrp);
        set_value(task_flags);
        set_value(min_flt);
        set_value(cmin_flt);
        set_value(maj_flt);
        set_value(cmaj_flt);
        set_value(utime);
        set_value(stime);
        set_value(cutime);
        set_value(cstime);
        set_value(priority);
        set_value(nice);
        set_value(num_threads);
        set_value(it_real_value);
        set_value(start_time);
        set_value(vsize);
        set_value(rss);
        set_value(rlim);
        set_value(start_code);
        set_value(end_code);
        set_value(start_stack);
        set_value(kstkesp);
        set_value(kstkeip);
        set_value(pendingsig);
        set_value(block_sig);
        set_value(sigign);
        set_value(sigcatch);
        set_value(wchan);
        set_value(nswap);
        set_value(cnswap);
        set_value(exit_signal);
        set_value(task_cpu);
        set_value(task_rt_priority);
        set_value(task_policy);
        out << division << std::endl;

        return out;
#define set_value
    }
};

} // namespace naesala

#ifndef _PERFCOUNT_H_
#define _PERFCOUNT_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

int initialize_counter (const __u32 event_type,const __u64 event_code);
void start_counter (const int fd);
void stop_counter (const int fd);
long long read_counter (const int fd);
int perf_event_open (struct perf_event_attr *hw_event, pid_t pid,
                      int cpu, int group_fd, unsigned long flags );

#endif /* _PERFCOUNT_H_ */

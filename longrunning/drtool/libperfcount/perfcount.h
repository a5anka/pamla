#ifndef _PERFCOUNT_H_
#define _PERFCOUNT_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <inttypes.h>
#include <stdint.h>

typedef uint64_t u64;

struct read_format {
   u64 value;         /* The value of the event */
   u64 time_enabled;  /* Perf total time enabled */
   u64 time_running;  /* Perf total time running */
};

struct output{
    u64 value;
    u64 scaled_value;   /* The scaled value of the event */
    float scale;
};

int initialize_counter (const __u32 event_type,const __u64 event_code);
void start_counter (const int fd);
void stop_counter (const int fd);
long long read_counter (const int fd);
int perf_event_open (struct perf_event_attr *hw_event, pid_t pid,
                      int cpu, int group_fd, unsigned long flags );
struct output get_output(const int fd);

#endif /* _PERFCOUNT_H_ */

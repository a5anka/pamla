#include "perfcount.h"

int initialize_counter(const __u32 event_type,const __u64 event_code)
{
   struct perf_event_attr pe;

   memset(&pe, 0, sizeof(struct perf_event_attr));
   pe.type = event_type;
   pe.size = sizeof(struct perf_event_attr);
   pe.config = event_code;
   pe.read_format = 3;
   pe.disabled = 1;
   pe.exclude_kernel = 1;
   pe.exclude_hv = 1;

   return perf_event_open(&pe, 0, -1, -1, 0);
}

void start_counter(const int fd)
{
   ioctl(fd, PERF_EVENT_IOC_RESET, 0);
   ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

void stop_counter(const int fd)
{
   ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
}

long long read_counter(const int fd)
{
   long long count;
   read(fd, &count, sizeof(long long));

   return count;
}

int perf_event_open( struct perf_event_attr *hw_event, pid_t pid,
                      int cpu, int group_fd, unsigned long flags )
{
   return syscall( __NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags );
}

struct output get_output(const int fd)
{
    struct read_format results;
    struct output out;

    read(fd, &results, sizeof(struct read_format));
    out.value = results.value;
    out.scale = results.time_running * 100.0 / results.time_enabled;
    out.scaled_value  = results.value * (results.time_enabled/results.time_running);

    return out;
}

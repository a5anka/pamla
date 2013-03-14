#include "perfcount.h"

#define GET_ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

struct perfcount_data *  perfcount_initialize_for_pid (pid_t pid)
{
   struct perfcount_data * pc_data =
      (struct perfcount_data *) malloc (sizeof(struct perfcount_data));

   pc_data->nr_events = 0;
   pc_data->pid = pid;

   return pc_data;
}

struct perfcount_data *  perfcount_initialize ()
{
   return perfcount_initialize_for_pid(DEFAULT_PID);
}

void perfcount_finalize (struct perfcount_data * pc_data)
{
   int i;

   for (i = 0; i < pc_data->nr_events; i++)
      {
         close(pc_data->file_descriptors[i]);
      }

   free(pc_data);
}

void perfcount_add_event (struct perfcount_data * pc_data,
                          const __u32 event_type,const __u64 event_code)
{
   pc_data->file_descriptors[pc_data->nr_events] =
      initialize_counter(event_type, event_code, pc_data->pid);

   ++pc_data->nr_events;
}

void perfcount_start(struct perfcount_data * pc_data)
{
   int i;

   for (i = 0; i < pc_data->nr_events; i++)
      {
         start_counter(pc_data->file_descriptors[i]);
         assert(pc_data->file_descriptors[i] >= 0);
      }
}

void perfcount_stop(struct perfcount_data * pc_data)
{
   int i;

   for (i = 0; i < pc_data->nr_events; i++)
      {
         stop_counter(pc_data->file_descriptors[i]);
      }
}

int initialize_counter(const __u32 event_type,const __u64 event_code, const pid_t pid)
{
   struct perf_event_attr pe;

   memset(&pe, 0, sizeof(struct perf_event_attr));
   pe.type = event_type;
   pe.size = sizeof(struct perf_event_attr);
   pe.config = event_code;
   pe.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
   pe.disabled = 1;
   pe.inherit = 1;
   pe.exclude_kernel = 1;
   pe.exclude_hv = 1;

   return perf_event_open(&pe, pid, -1, -1, 0);
}

void start_counter(const int fd)
{
   ioctl(fd, PERF_EVENT_IOC_RESET);
   ioctl(fd, PERF_EVENT_IOC_ENABLE);
}

void stop_counter(const int fd)
{
   ioctl(fd, PERF_EVENT_IOC_DISABLE);
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
   if (results.time_running > 0)
      {
         out.scaled_value  = results.value * (results.time_enabled/results.time_running);
      }
   else
      {
         out.scaled_value  = 0;
      }

   return out;
}

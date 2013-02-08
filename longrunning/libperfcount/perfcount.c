#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

int initialize_counter(const __u32 event_type,const __u64 event_code);
void start_counter(const int fd);
void stop_counter(const int fd);
long long read_counter(const int fd);
long perf_event_open( struct perf_event_attr *hw_event, pid_t pid,
                      int cpu, int group_fd, unsigned long flags );

int main(int argc, char **argv)
{
   long long count;

   int fd = initialize_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);

   if (fd < 0)
      {
         fprintf(stderr, "Error initializing counter.\n");
      }

   start_counter(fd);

   printf("Measuring instruction count for this printf\n");

   stop_counter(fd);
   read(fd, &count, sizeof(long long));

   printf("Used %lld instructions\n", count);

   close(fd);
}

int initialize_counter(const __u32 event_type,const __u64 event_code)
{
   struct perf_event_attr pe;
   int fd;

   memset(&pe, 0, sizeof(struct perf_event_attr));
   pe.type = event_type;
   pe.size = sizeof(struct perf_event_attr);
   pe.config = event_code;
   pe.disabled = 1;
   pe.exclude_kernel = 1;
   pe.exclude_hv = 1;

   fd = perf_event_open(&pe, 0, -1, -1, 0);
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

long perf_event_open( struct perf_event_attr *hw_event, pid_t pid,
                      int cpu, int group_fd, unsigned long flags )
{
   return syscall( __NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags );
}

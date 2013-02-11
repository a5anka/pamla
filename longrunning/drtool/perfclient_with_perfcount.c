#include "dr_api.h"
#include "perfcount.h"

void event_exit(void);
void print_to_err(char* buf);
void event_thread_init(void *drcontext);
void event_thread_exit(void *drcontext);
void start_perf();
void stop_perf();

int fd1, fd2,fd3,fd4,fd5;

DR_EXPORT void dr_init(client_id_t id)
{
   dr_register_exit_event(event_exit);

   fd1 = initialize_counter(PERF_TYPE_RAW, 0x00c0);
   fd2 = initialize_counter(PERF_TYPE_RAW, 0x02f1);
   fd3 = initialize_counter(PERF_TYPE_RAW, 0x0126);
   fd4 = initialize_counter(PERF_TYPE_RAW, 0x0227);
   fd5 = initialize_counter(PERF_TYPE_RAW, 0x08a2);

   start_counter(fd1);
   start_counter(fd2);
   start_counter(fd3);
   start_counter(fd4);
   start_counter(fd5);

   print_to_err("DR Client is starting... \n");
}

void event_exit(void)
{
   long long count;

   stop_counter(fd1);
   stop_counter(fd2);
   stop_counter(fd3);
   stop_counter(fd4);
   stop_counter(fd5);

   read(fd1, &count, sizeof(long long));
   printf("\nInstructions retired (0x00c0) %lld\n", count);
   read(fd2, &count, sizeof(long long));
   printf("Event (0x02f1) %lld\n", count);
   read(fd3, &count, sizeof(long long));
   printf("Event (0x0126) %lld\n", count);
   read(fd4, &count, sizeof(long long));
   printf("Event (0x0227) %lld\n", count);
   read(fd5, &count, sizeof(long long));
   printf("Event (0x08a2) %lld\n\n", count);

   print_to_err("DR Client is exiting...\n");
}

void start_perf()
{
   print_to_err("Starting perf...\n");
}

void stop_perf()
{
   print_to_err("Stopping perf...\n");
}

void print_to_err(char *message)
{
   if (dr_is_notify_on())
      {
         dr_fprintf(STDERR, "%s", message);
      }
}

#include "dr_api.h"
#include "perfcount.h"

void event_exit(void);
void print_to_err(char* buf);
void event_thread_init(void *drcontext);
void event_thread_exit(void *drcontext);
void start_perf();
void stop_perf();

int fd1,fd2,fd3,fd4,fd5,fd6,fd7,fd8,fd9,fd10,fd11,fd12,fd13,fd14,fd15,fd16;

DR_EXPORT void dr_init(client_id_t id)
{
   dr_register_exit_event(event_exit);

   fd1 = initialize_counter(PERF_TYPE_RAW, 0x00c0);
   fd2 = initialize_counter(PERF_TYPE_RAW, 0x0126);
   fd3 = initialize_counter(PERF_TYPE_RAW, 0x0227);
   fd4 = initialize_counter(PERF_TYPE_RAW, 0x0224);
   fd5 = initialize_counter(PERF_TYPE_RAW, 0x08a2);
   fd6 = initialize_counter(PERF_TYPE_RAW, 0x01b0);
   fd7 = initialize_counter(PERF_TYPE_RAW, 0x20f0);
   fd8 = initialize_counter(PERF_TYPE_RAW, 0x02f1);
   fd9 = initialize_counter(PERF_TYPE_RAW, 0x01f2);
   fd10 = initialize_counter(PERF_TYPE_RAW, 0x01b8);
   fd11 = initialize_counter(PERF_TYPE_RAW, 0x02b8);
   fd12 = initialize_counter(PERF_TYPE_RAW, 0x04b8);
   fd13 = initialize_counter(PERF_TYPE_RAW, 0x40cb);
   fd14 = initialize_counter(PERF_TYPE_RAW, 0x0149);
   fd15 = initialize_counter(PERF_TYPE_RAW, 0x0151);
   fd16 = initialize_counter(PERF_TYPE_RAW, 0x02a2);

   start_counter(fd1);
   start_counter(fd2);
   start_counter(fd3);
   start_counter(fd4);
   start_counter(fd5);
   start_counter(fd6);
   start_counter(fd7);
   start_counter(fd8);
   start_counter(fd9);
   start_counter(fd10);
   start_counter(fd11);
   start_counter(fd12);
   start_counter(fd13);
   start_counter(fd14);
   start_counter(fd15);
   start_counter(fd16);

   print_to_err("DR Client is starting... \n");
}

void event_exit(void)
{
   struct read_format results;

   stop_counter(fd1);
   stop_counter(fd2);
   stop_counter(fd3);
   stop_counter(fd4);
   stop_counter(fd5);
   stop_counter(fd6);
   stop_counter(fd7);
   stop_counter(fd8);
   stop_counter(fd9);
   stop_counter(fd10);
   stop_counter(fd11);
   stop_counter(fd12);
   stop_counter(fd13);
   stop_counter(fd14);
   stop_counter(fd15);
   stop_counter(fd16);

   read(fd1, &results.value, sizeof(struct read_format));
   printf("\nInstructions retired (0x00c0) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd2, &results.value, sizeof(struct read_format));
   printf("Event (0x0126) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd3, &results.value, sizeof(struct read_format));
   printf("Event (0x0227) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd4, &results.value, sizeof(struct read_format));
   printf("Event (0x0224) %" PRIu64
          " (%f%%)\n",
       results.value, results.time_running*100.0/results.time_enabled);
   read(fd5, &results.value, sizeof(struct read_format));
   printf("Event (0x08a2) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd6, &results.value, sizeof(struct read_format));
   printf("Event (0x01b0) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd7, &results.value, sizeof(struct read_format));
   printf("Event (0x20f0) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd8, &results.value, sizeof(struct read_format));
   printf("Event (0x02f1) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd9, &results.value, sizeof(struct read_format));
   printf("Event (0x01f2) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd10, &results.value, sizeof(struct read_format));
   printf("Event (0x01b8) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd11, &results.value, sizeof(struct read_format));
   printf("Event (0x02b8) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd12, &results.value, sizeof(struct read_format));
   printf("Event (0x04b8) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd13, &results.value, sizeof(struct read_format));
   printf("Event (0x40cb) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd14, &results.value, sizeof(struct read_format));
   printf("Event (0x0149) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd15, &results.value, sizeof(struct read_format));
   printf("Event (0x0151) %" PRIu64
          " (%f%%)\n",
          results.value, results.time_running*100.0/results.time_enabled);
   read(fd16, &results.value, sizeof(struct read_format));
   printf("Event (0x02a2) %" PRIu64
          " (%f%%)\n\n",
          results.value, results.time_running*100.0/results.time_enabled);

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

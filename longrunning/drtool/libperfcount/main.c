#include "perfcount.h"

int fd1,fd2,fd3,fd4,fd5,fd6,fd7,fd8,fd9,fd10,fd11,fd12,fd13,fd14,fd15,fd16;

int main(int argc, char **argv)
{
   struct read_format results;

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

   /* if (fd < 0) */
   /*    { */
   /*       fprintf(stderr, "Error initializing counter.\n"); */
   /*    } */


   printf("Measuring instruction count for this printf\n");

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
   read(fd1, &results, sizeof(results));

   printf("Instructions %" PRIu64 "\n" , results.value);
   printf("Time enabled %" PRIu64 "\n", results.time_enabled);
   printf("Time running %" PRIu64 "\n", results.time_running);

   close(fd1);
}

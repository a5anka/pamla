#include "perfcount.h"

int main(int argc, char **argv)
{
   long long count;

   int fd = initialize_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);

   printf ("fd value %d\n", fd);

   if (fd < 0)
      {
         fprintf(stderr, "Error initializing counter.\n");
      }

   start_counter(fd);

   printf("Measuring instruction count for this printf\n");
   sleep(3);

   stop_counter(fd);
   read(fd, &count, sizeof(long long));

   printf("Used %lld instructions\n", count);

   close(fd);
}

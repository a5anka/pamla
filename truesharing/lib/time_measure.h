#include <time.h>

void saveCurrentTime(struct timespec *timeHolder) {
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, timeHolder);
}

float elapsedTime(struct timespec *begin, struct timespec *end)
{
   unsigned long nanoSeconds, seconds;

   if (end->tv_nsec < begin->tv_nsec) {
      nanoSeconds = 1000000000 - (begin->tv_nsec - end->tv_nsec);
      seconds  = end->tv_sec - begin->tv_sec - 1;
   }
   else {
      nanoSeconds = end->tv_nsec - begin->tv_nsec;
      seconds  = end->tv_sec - begin->tv_sec;
   }

   return (float) (seconds) * 1000 + ((float) (nanoSeconds)) / 1000000;
}

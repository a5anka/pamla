#include "dr_api.h"

void event_exit(void);
void print_to_err(char* buf);
void event_thread_init(void *drcontext);
void event_thread_exit(void *drcontext);
void start_perf();
void stop_perf();

int thread_count;
void *count_mutex;

DR_EXPORT void dr_init(client_id_t id)
{
   thread_count = 0;

   dr_register_exit_event(event_exit);
   dr_register_thread_init_event (event_thread_init);
   dr_register_thread_exit_event (event_thread_exit);

   print_to_err("DR Client is starting... \n");

   count_mutex = dr_mutex_create();
}

void event_exit(void)
{
   print_to_err("DR Client is exiting...\n");
}

void event_thread_init(void *drcontext)
{
   dr_mutex_lock(count_mutex);
   if (thread_count++ == 1)
      {
         start_perf();
      }
   dr_mutex_unlock(count_mutex);
}

void event_thread_exit(void *drcontext)
{
   dr_mutex_lock(count_mutex);
   if (--thread_count == 1)
      {
         stop_perf();
      }
   dr_mutex_unlock(count_mutex);
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

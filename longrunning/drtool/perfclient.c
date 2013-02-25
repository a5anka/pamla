#include "dr_api.h"
#include "perfcount.h"

void event_exit(void);
void print_to_err(char* buf);
void event_thread_init(void *drcontext);
void event_thread_exit(void *drcontext);
void initialize_perf();
void start_perf();
void stop_perf();

int thread_count;
pid_t pid;
void *count_mutex;
int fd1,fd2,fd3,fd4,fd5,fd6,fd7,fd8,fd9,fd10,fd11,fd12,fd13,fd14,fd15,fd16;

DR_EXPORT void dr_init(client_id_t id)
{
   thread_count = 0;
   pid = getpid();
   dr_register_exit_event(event_exit);
   dr_register_thread_init_event (event_thread_init);
   dr_register_thread_exit_event (event_thread_exit);
   initialize_perf();
   start_perf();
   print_to_err("DR Client is starting... \n");

   count_mutex = dr_mutex_create();
}

void event_exit(void)
{
   print_to_err("DR Client is exiting...\n");
   stop_perf();
}

void event_thread_init(void *drcontext)
{
   dr_mutex_lock(count_mutex);
   if (thread_count++ == 1)
      {
         /* stop_perf(); */
      }
   dr_mutex_unlock(count_mutex);
}

void event_thread_exit(void *drcontext)
{
   dr_mutex_lock(count_mutex);
   if (--thread_count == 1)
      {
         /* start_perf(); */
      }
   dr_mutex_unlock(count_mutex);

}

void initialize_perf()
{
   fd1 = initialize_counter(PERF_TYPE_RAW, 0x00c0, pid);
   fd2 = initialize_counter(PERF_TYPE_RAW, 0x0126, pid);
   fd3 = initialize_counter(PERF_TYPE_RAW, 0x0227, pid);
   fd4 = initialize_counter(PERF_TYPE_RAW, 0x0224, pid);
   fd5 = initialize_counter(PERF_TYPE_RAW, 0x08a2, pid);
   fd6 = initialize_counter(PERF_TYPE_RAW, 0x01b0, pid);
   fd7 = initialize_counter(PERF_TYPE_RAW, 0x20f0, pid);
   fd8 = initialize_counter(PERF_TYPE_RAW, 0x02f1, pid);
   fd9 = initialize_counter(PERF_TYPE_RAW, 0x01f2, pid);
   fd10 = initialize_counter(PERF_TYPE_RAW, 0x01b8, pid);
   fd11 = initialize_counter(PERF_TYPE_RAW, 0x02b8, pid);
   fd12 = initialize_counter(PERF_TYPE_RAW, 0x04b8, pid);
   fd13 = initialize_counter(PERF_TYPE_RAW, 0x40cb, pid);
   fd14 = initialize_counter(PERF_TYPE_RAW, 0x0149, pid);
   fd15 = initialize_counter(PERF_TYPE_RAW, 0x0151, pid);
   fd16 = initialize_counter(PERF_TYPE_RAW, 0x02a2, pid);
}

void start_perf()
{
   print_to_err("Starting perf...\n");

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
}

void stop_perf()
{
   print_to_err("Stopping perf...\n");

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

   struct output out;

   out = get_output(fd1);
   dr_printf("\nInstructions retired (0x00c0) %" PRIu64
          " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd2);
   dr_printf("Event (0x0126) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd3);
   dr_printf("Event (0x0227) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd4);
   dr_printf("Event (0x0224) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd5);
   dr_printf("Event (0x08a2) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd6);
   dr_printf("Event (0x01b0) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd7);
   dr_printf("Event (0x20f0) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd8);
   dr_printf("Event (0x02f1) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd9);
   dr_printf("Event (0x01f2) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd10);
   dr_printf("Event (0x01b8) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd11);
   dr_printf("Event (0x02b8) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd12);
   dr_printf("Event (0x04b8) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd13);
   dr_printf("Event (0x40cb) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd14);
   dr_printf("Event (0x0149) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd15);
   dr_printf("Event (0x0151) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(fd16);
   dr_printf("Event (0x02a2) %" PRIu64
             " (%f%%)\n\n",
             out.scaled_value, out.scale);

   close(fd1);
   close(fd2);
   close(fd3);
   close(fd4);
   close(fd5);
   close(fd6);
   close(fd7);
   close(fd8);
   close(fd9);
   close(fd10);
   close(fd11);
   close(fd12);
   close(fd13);
   close(fd14);
   close(fd15);
   close(fd16);
}

void print_to_err(char *message)
{
   if (dr_is_notify_on())
      {
         dr_fprintf(STDERR, "%s", message);
      }
}

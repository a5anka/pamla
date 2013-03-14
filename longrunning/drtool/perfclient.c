#include "dr_api.h"
#include "perfcount.h"

void event_exit(void);
void print_to_err(char* buf);
void event_thread_init(void *drcontext);
void event_thread_exit(void *drcontext);
void initialize_perf();
void start_perf();
void stop_perf();

static int thread_count;
static pid_t pid;
static void *count_mutex;
static struct perfcount_data * pc_data;

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
   pc_data = perfcount_initialize_for_pid(pid);

   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x00c0);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x0126);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x0227);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x0224);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x08a2);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x01b0);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x20f0);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x02f1);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x01f2);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x01b8);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x02b8);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x04b8);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x40cb);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x0149);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x0151);
   perfcount_add_event(pc_data, PERF_TYPE_RAW, 0x02a2);
}

void start_perf()
{
   print_to_err("Starting perf...\n");

   perfcount_start(pc_data);
}

void stop_perf()
{
   print_to_err("Stopping perf...\n");

   perfcount_stop(pc_data);

   struct output out;

   out = get_output(pc_data->file_descriptors[0]);
   dr_printf("\nInstructions retired (0x00c0) %" PRIu64
          " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[1]);
   dr_printf("Event (0x0126) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[2]);
   dr_printf("Event (0x0227) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[3]);
   dr_printf("Event (0x0224) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[4]);
   dr_printf("Event (0x08a2) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[5]);
   dr_printf("Event (0x01b0) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[6]);
   dr_printf("Event (0x20f0) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[7]);
   dr_printf("Event (0x02f1) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[8]);
   dr_printf("Event (0x01f2) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[9]);
   dr_printf("Event (0x01b8) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[10]);
   dr_printf("Event (0x02b8) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[11]);
   dr_printf("Event (0x04b8) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[12]);
   dr_printf("Event (0x40cb) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[13]);
   dr_printf("Event (0x0149) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[14]);
   dr_printf("Event (0x0151) %" PRIu64
             " (%f%%)\n",
             out.scaled_value, out.scale);

   out = get_output(pc_data->file_descriptors[15]);
   dr_printf("Event (0x02a2) %" PRIu64
             " (%f%%)\n\n",
             out.scaled_value, out.scale);

}

void print_to_err(char *message)
{
   if (dr_is_notify_on())
      {
         dr_fprintf(STDERR, "%s", message);
      }
}

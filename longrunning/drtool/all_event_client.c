#include "dr_api.h"

static void event_exit(void);
static dr_emit_flags_t event_basic_block(void *drcontext,
                                         void *tag, instrlist_t *bb,
                                         bool for_trace, bool translating);
static dr_emit_flags_t event_trace_block(void *drcontext,
                                         void *tag, instrlist_t *bb,
                                         bool translating);
static void event_delete_frag(void *drcontext, void *tag);
static dr_custom_trace_action_t event_end_trace(void *drcontext, void *tag, void *next_tag);
static void event_fork_init(void *drcontext);
static void event_thread_exit(void *drcontext);

static int bb_count;
static int delete_count;

DR_EXPORT void
dr_init(client_id_t id)
{
   bb_count = 0;

   dr_register_thread_init_event (event_thread_exit);
   dr_register_exit_event(event_exit);

   if (dr_is_notify_on())
      {
         dr_fprintf (STDERR, "My Client is starting...\n");
      }
}

static void
event_exit(void)
{
   dr_printf("Count is %d\n", delete_count);

   if (dr_is_notify_on())
      {
         dr_fprintf (STDERR, "My Client is exiting...\n");
      }
}

static dr_emit_flags_t
event_basic_block(void *drcontext, void *tag, instrlist_t *bb,
                  bool for_trace, bool translating)
{
   ++bb_count;
}

static dr_emit_flags_t
event_trace_block(void *drcontext, void *tag, instrlist_t *bb,
                   bool translating)
{
   if (!translating)
      {
         bb_count++;
         dr_fprintf(STDERR, "Tace block ... \n");
      }

}

static void event_delete_frag(void *drcontext, void *tag)
{
   ++delete_count;
}

static dr_custom_trace_action_t event_end_trace(void *drcontext, void *tag, void *next_tag)
{
   ++delete_count;
   return CUSTOM_TRACE_DR_DECIDES;
}

static void event_fork_init(void *drcontext)
{
   ++bb_count;
}

static void event_thread_exit(void *drcontext)
{
   ++delete_count;
}

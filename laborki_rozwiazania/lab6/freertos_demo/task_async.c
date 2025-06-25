#include "task_async.h"
#include "tasks_config.h"

static async_context_freertos_t async_context_instance;

// Create an async context
async_context_t *example_async_context(void) {
    async_context_freertos_config_t config = async_context_freertos_default_config();
    config.task_priority = WORKER_TASK_PRIORITY; // defaults to ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_PRIORITY
    config.task_stack_size = WORKER_TASK_STACK_SIZE; // defaults to ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_STACK_SIZE
    if (!async_context_freertos_init(&async_context_instance, &config))
        return NULL;
    return &async_context_instance.core;
}

// async workers run in their own thread when using async_context_freertos_t with priority WORKER_TASK_PRIORITY
static void do_work(async_context_t *context, async_at_time_worker_t *worker) {
    async_context_add_at_time_worker_in_ms(context, worker, 10000);
    static uint32_t count = 0;
    printf("Hello from worker count=%u\n", count++);
    static int last_core_id = -1;
    if (portGET_CORE_ID() != last_core_id) {
        last_core_id = portGET_CORE_ID();
        printf("worker is on core %d\n", last_core_id);
    }
}

async_at_time_worker_t worker_timeout = { .do_work = do_work };

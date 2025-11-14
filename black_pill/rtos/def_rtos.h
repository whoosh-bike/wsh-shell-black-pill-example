#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "rtos_tasks_stack_and_prio.h"

#define RTOS_MIN_TIMEOUT_MS  20
#define RTOS_LONG_TIMEOUT_MS 1000

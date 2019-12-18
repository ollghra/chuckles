#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <stdint.h>

typedef struct TCB{
	uint32_t kstack_top;
	uint32_t cr3;
} task_control_block_t;

void switch_to_task(task_control_block_t *next_task);

#endif

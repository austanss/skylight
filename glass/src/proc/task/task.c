#include <stdlib.h>
#include <string.h>
#include "proc/task/task.h"
#include "mm/paging/paging.h"
#include "mm/pmm/pmm.h"

typedef struct _linked_task {
    task_t* task;
    struct _linked_task* next;
} linked_task_t;

linked_task_t   *tasks;
uint64_t        task_count;
uint64_t        current_task_id;

// PRECONFIGURES scheduler, scheduler starts when first task is added
void scheduler_preconfigure()
{
    task_count = 0;
}

// Creates a new page table for a tasks context
paging_table_t* task_new_page_table() {
    paging_table_t* task_table = (paging_table_t *)pmm_alloc_page();
    memset(task_table, 0, PAGING_PAGE_SIZE); // zero out page table or else weird page faults will occur
    return task_table;
}

#define MEGABYTE 1024*1024*1024

// Creates a new stack of a megabyte for a task
uint64_t task_new_stack() {
    return (uint64_t)pmm_alloc_pool(MEGABYTE / PAGING_PAGE_SIZE) + MEGABYTE;
}

uint64_t task_create_new(void* program_entry) {
    task_t* new_task = (task_t *)malloc(sizeof(task_t));
    
    new_task->id = task_count;
    new_task->priority = 0;
    new_task->state = TASK_STATE_SLEEPING;  // create it sleeping, start it later

    new_task->ctx = (task_context_t *)malloc(sizeof(task_context_t));
    new_task->ctx->registers.rdi = 0;
    new_task->ctx->registers.rsi = 0;
    new_task->ctx->registers.rdx = 0;
    new_task->ctx->registers.rcx = 0;
    new_task->ctx->registers.rbx = 0;
    new_task->ctx->registers.rax = 0;
    new_task->ctx->registers.r8 = 0;
    new_task->ctx->registers.r9 = 0;
    new_task->ctx->registers.r10 = 0;
    new_task->ctx->registers.r11 = 0;
    new_task->ctx->registers.r12 = 0;
    new_task->ctx->registers.r13 = 0;
    new_task->ctx->registers.r14 = 0;
    new_task->ctx->registers.r15 = 0;


    new_task->ctx->stack.rsp = task_new_stack();
    new_task->ctx->stack.rbp = new_task->ctx->stack.rsp;
    
    new_task->ctx->rip = (uint64_t)program_entry;
    new_task->ctx->cr3 = (uint64_t)task_new_page_table();
    
    // add task to linked list

    linked_task_t* new_link = (linked_task_t *)tasks;

    if (tasks == NULL) {
        tasks = (linked_task_t *)malloc(sizeof(linked_task_t));
        tasks->task = new_task;
        tasks->next = NULL;
    } else {
        while (new_link->next != NULL) {
            new_link = new_link->next;
        }
        new_link->next = (linked_task_t *)malloc(sizeof(linked_task_t));
        new_link->next->task = new_task;
        new_link->next->next = NULL;
    }
    
    task_count++;

    return new_task->id;
}

void task_schedule(uint64_t task_id) {
    linked_task_t* thread = tasks;
    while (thread != NULL) {
        if (thread->task->id == task_id) {
            thread->task->state = TASK_STATE_WAITING;
            break;
        }
        thread = thread->next;
    }
}

void task_sleep(uint64_t task_id) {
    linked_task_t* thread = tasks;
    while (thread != NULL) {
        if (thread->task->id == task_id) {
            thread->task->state = TASK_STATE_SLEEPING;
            break;
        }
        thread = thread->next;
    }
}

void task_kill(uint64_t task_id) {
    linked_task_t* thread = tasks;
    while (thread != NULL) {
        if (thread->task->id == task_id) {
            thread->task->state = TASK_STATE_DEAD;
            break;
        }
        thread = thread->next;
    }
}

extern void _load_task_page_tables(void* cr3);

extern void _finalize_task_switch(task_context_t* ctx);

void task_select(uint64_t task_id) {
    current_task_id = task_id;

    linked_task_t* thread = tasks;

    while (thread != NULL) {
        if (thread->task->id == task_id)
            break;
        thread = thread->next; 
    }

    _load_task_page_tables((void *)thread->task->ctx->cr3);

    _finalize_task_switch(thread->task->ctx);
}


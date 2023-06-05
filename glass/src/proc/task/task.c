#include <stdlib.h>
#include <string.h>
#include "proc/task/task.h"
#include "mm/paging/paging.h"
#include "cpu/tss/tss.h"
#include "mm/pmm/pmm.h"
#include "dev/io.h"
#include "dev/uart/serial.h"

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

extern uint8_t __load_base;
extern uint8_t __load_max;

extern uint8_t* stack;

#define MEGABYTE 1024*1024

// Creates a new page table for a tasks context
paging_table_t* task_new_page_table(elf_load_info_t* load_info, task_t* task) {
    paging_table_t* task_table = (paging_table_t *)pmm_alloc_page();
    memset(task_table, 0, PAGING_PAGE_SIZE); // zero out page table or else weird page faults will occur

    paging_table_t* kpml4 = paging_get_pml4();
    paging_load_pml4(task_table);

    for (uint64_t t = (uint64_t)&__load_base; t < (uint64_t)&__load_max; t+=PAGING_PAGE_SIZE)
        paging_map_page((void *)t, (void *)(t - PAGING_KERNEL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);

    for (uint64_t i = 0; i < load_info->segment_count; i++) {
        for (uint64_t j = 0; j < load_info->segments[i].length; j+=PAGING_PAGE_SIZE)
            paging_map_page((void *)(load_info->segments[i].loaded_at + j), (void *)(load_info->segments[i].located_at + j), PAGING_FLAGS_USER_PAGE);
    }

    for (uint64_t head = (uint64_t)stack; head < (uint64_t)stack + (16 * PAGING_PAGE_SIZE); head+=PAGING_PAGE_SIZE)     // map current kernel stack
        paging_map_page((void *)head, (void *)(head - PAGING_KERNEL_OFFSET), PAGING_FLAGS_KERNEL_PAGE);
        
    for (uint64_t head = task->ctx->stack.rsp - MEGABYTE; head < task->ctx->stack.rsp; head+=PAGING_PAGE_SIZE)     // map current kernel stack
        paging_map_page((void *)head, (void *)(head - PAGING_VIRTUAL_OFFSET), PAGING_FLAGS_USER_PAGE);

    return task_table;
}

// Creates a new stack of a megabyte for a task
uint64_t task_new_stack() {
    return (uint64_t)pmm_alloc_pool(MEGABYTE / PAGING_PAGE_SIZE) + MEGABYTE;
}

gs_kernel_base_t* task_new_gs_base() {
    gs_kernel_base_t* gs_base = (gs_kernel_base_t *)pmm_alloc_page();
    memset(gs_base, 0, PAGING_PAGE_SIZE);
    return gs_base;
}


uint64_t task_create_new(elf_load_info_t* load_info) {
    task_t* new_task = (task_t *)malloc(sizeof(task_t));

    serial_terminal()->puts("\n\nCreating new task from ")->putul((uint64_t)load_info->entry)->puts("...\n");
    
    serial_terminal()->puts("\tSetting metadata...\n");
    new_task->id = task_count;
    new_task->priority = 0;
    new_task->state = TASK_STATE_SLEEPING;  // create it sleeping, start it later

    serial_terminal()->puts("\tCreating GS base structure...\n");
    new_task->gs_base = task_new_gs_base();
    new_task->gs_base->tss = 0;
    new_task->gs_base->pid = new_task->id;
    new_task->gs_base->rsp = tss_get(0)->rsp[0];
    new_task->gs_base->pc = (uint64_t)load_info->entry;

    serial_terminal()->puts("\tCreating new task context...\n");
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

    serial_terminal()->puts("\tAllocating new stack...\n");
    new_task->ctx->stack.rsp = task_new_stack();
    new_task->ctx->stack.rbp = new_task->ctx->stack.rsp;
    
    serial_terminal()->puts("\tCreating page tables...\n");
    new_task->ctx->rip = (uint64_t)load_info->entry;
    new_task->ctx->cr3 = (uint64_t)task_new_page_table(load_info, new_task);
    
    // add task to linked list

    serial_terminal()->puts("Registering task... ");
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

    serial_terminal()->puts("done.\n");

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

#define IA32_KERNEL_GS_BASE 0xC0000102
#define IA32_GS_BASE        0xC0000101

void task_select(uint64_t task_id) {
    current_task_id = task_id;

    serial_terminal()->puts("Selecting task ")->putd(task_id)->puts("...\n");

    linked_task_t* thread = tasks;

    while (thread != NULL) {
        if (thread->task->id == task_id)
            break;
        thread = thread->next; 
    }

    wrmsr(IA32_KERNEL_GS_BASE, (uint64_t)thread->task->gs_base);
    wrmsr(IA32_GS_BASE, 0);

    __asm__ volatile ("cli");

    _load_task_page_tables((void *)thread->task->ctx->cr3);

    _finalize_task_switch(thread->task->ctx);
}


#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    struct {
        uint64_t    rdi;
        uint64_t    rsi;
        uint64_t    rdx;
        uint64_t    rcx;
        uint64_t    rbx;
        uint64_t    rax;
    } registers;
    
    struct {
        uint64_t top;
        uint64_t base;
    } stack;

    uint64_t head;
    uint64_t page_table;
    
} task_context_t;

typedef struct {
    uint64_t id;
    task_context_t* ctx;
} task_t;
